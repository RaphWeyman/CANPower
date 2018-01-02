/* 
 * File:   application.c
 * Author: Raph Weyman
 *
 * Created on 15 November 2017, 11:37
 * 
 * CAN Power application.
 * Requires the initialisation function to be called once after all drivers and services have
 * been initialised.
 * ApplicationTasks must be invoked repeatedly at least once per timer tick.
 * 
 * Switch channel 1 is set to go fully on or fully off with the ignition for accessory power
 * (can easily be changed to staying off by changing the CHANNEL_1_ON define).
 * 
 * Channel 0 is controlled by the ASC switch (can be changed with the BUTTON_PRESSED define).
 * Channel 0 has two modes; modulated and unmodulated. Unmodulated behaves like channel 1; fully on
 * or fully off with the ignition.
 * Modulated is PWM controlled for several power levels from fully off to fully on and each of the quarters
 * inbetween (modify at the CHANNEL_0_PWM_SETTING). PWM is slow for heated clothing but can be changed to
 * fast by changing the CHANNEL_0_PWM_MODE.
 * A long press (>20 seconds) of the control button toggles between modulated and unmodulated modes.
 * A short press (<1 second) of the control button while in modulated mode cycles through the modulation levels.
 * Channel 0 mode and modulation level are stored in EEPROM at power off time (immmediately prior to transition to the
 * alarm simulation state)).
 * 
 * An alarm simulation is included after the ignition is turned off (and the POWER_OFF_DELAY has elapsed.
 * During alarm simulation the switch chip is turned off but the LED indicates as if there were an
 * alarm module armed.
 * Alarm simulation continues for the ALARM_SIMULATION_TIME or can be disabled by setting the ALARM_SIMULATION_TIME to zero.
 * A kickstand warning indication is output to the LEDs if the kickstand is deployed while the ignition is on
 * The warning can be disabled by commenting out the KICKSTAND_WARNING define.
 * 
 * Ignition on is determined by CAN messages being received.
 * Fully on state is maintained POWER_OFF_DELAY. If no CAN messages are received for longer than the POWER_OFF_DELAY
 * then channels 0 and 1 are both turned off and the alarm simulation is stated.
 * Power to the CPU is latched using the PORT_POWER hardware control. When either the ignition is on or the PORT_POWER is set to
 * POWER_PORT_ON power is supplied to the CPU. This allows the CPU to maintain power during the POWER_OFF_DELAY
 * and the ALARM_SIMLATION_TIME. At the end of the ALARM_SIMULATION_TIME, the PORT_POWER is set to POWER_PORT_OFF so as to cut
 * power.
 * 
 * Indications are assumed to be for a single bi-colour LED.
 * LED0 fully on indicates channel 0 is in unmodulated mode.
 * LED1 on but with between 1 and 4 inverse blips indicates channel 0 modulated mode and the number of inverse blips indicate
 * the modulation level (4 blips - fully on).
 * LED1 flashes 4 blips when the channel 0 mode is changed.
 * Both LED0 and LED1 fully off indicate either modulated mode with channel 0 output at the fully off level or no power to the CPU.
 * LED0 flashing at 1Hz indicates that the kickstand is extended.
 * LED0 flashing at 4Hz indicates a switch chip fault - recover by ignition off/ignition on.
 * LED1 blips every few seconds for the alarm simulation.
 * Indications can all be changed by modifying the INDICATIONS and CHANNEL_0_INDICATIONS table.
 *
 * Application is a state machine with a single state variable 'state' and a function of type
 * state_function_t for each state.
 * Each state function must implement the actions for state entry and for state maintenance.
 * The current state's function is invoked regularly (once per timer tick).
 * The StateTransition function is used to change state.
 * The state machnine can be extended by adding to BOTH the states_t and the STATE_FUNCTIONS
 * 
 */

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include "application.h"
#include "timer.h"
#include "LEDs.h"
#include "CAN.h"
#include "MC06XSD200.h"
#include "EEPROM.h"
#include "ports.h"

// Delay after last CAN message for which the fully on state is maintained
#define POWER_OFF_DELAY (3*TICKS_PER_MINUTE) // 3 minutes

// If no CAN messages are received for this amount of time then the ignition is assumed to be off
#define IGNITION_OFF_DELAY (1000/TIMER_PERIOD) // 1 second

// Alarm simulation indicated after turning off for this many minutes
// Set to zero for no alarm simulation indication - goes straight to power removal
// Alarm simulation draws about 8mA and so would fully flatten a battery within a month
// or two.
#define ALARM_SIMULATION_TIME (12*60) // 12 hours

// If defined then the kickstand warning indication will be issued with the ignition is on and the
// kickstand is deployed.
// Comment out to disable
#define KICKSTAND_WARNING


// the modes that the power switch accommodates - either unmodulated always full on or PWM modulated
typedef enum {MODE_MODULATED=0, MODE_UNMODULATED, NUMBER_OF_POWER_MODES} switch_mode_t;

// the power levels that can be modulated to
typedef enum {MODULATED_OFF=0, MODULATED_QUARTER, MODULATED_HALF, MODULATED_THREE_QUARTER, MODULATED_MAXIMUM, NUMBER_OF_MODULATED_POWER_LEVELS} modulated_power_level_t;

// EEPROM locations
typedef enum {MODE_EEPROM_ADDRESS=0, MODULATION_LEVEL_EEPROM_ADDRESS} EEPROM_address_t;


// channel 0 output levels for each of the configurable modes
#define CHANNEL_0_OFF PWM_FULL_OFF
#define CHANNEL_0_OFF_PWM_MODE FAST_PWM
static const PWM_mode_t CHANNEL_0_PWM_MODE[NUMBER_OF_POWER_MODES][NUMBER_OF_MODULATED_POWER_LEVELS] =
    {{SLOW_PWM, SLOW_PWM, SLOW_PWM, SLOW_PWM},
    {FAST_PWM, FAST_PWM, FAST_PWM, FAST_PWM}};
static const uint16_t CHANNEL_0_PWM_SETTING[NUMBER_OF_POWER_MODES][NUMBER_OF_MODULATED_POWER_LEVELS] =
    {{PWM_FULL_OFF, PWM_FULL_ON/4, PWM_FULL_ON/2, PWM_FULL_ON*3/4, PWM_FULL_ON},
    {PWM_FULL_ON, PWM_FULL_ON, PWM_FULL_ON, PWM_FULL_ON}};


// channel 1 output levels when control power off and when on - currently set to turn on and off with the controller 
#define CHANNEL_1_OFF PWM_FULL_OFF
#define CHANNEL_1_OFF_PWM_MODE FAST_PWM
#define CHANNEL_1_ON PWM_FULL_ON
#define CHANNEL_1_ON_PWM_MODE FAST_PWM

typedef struct
{
    LED_pattern_t LED0_pattern;
    LED_pattern_t LED1_pattern;
} LED_combination_t;

typedef enum {ALARM_INDICATION=0, KICKSTAND_INDICATION, UNMODULATED_ON_INDICATION, MODULATED_OFF_INDICATION,
    MODULATED_QUARTER_INDICATION, MODULATED_HALF_INDICATION, MODULATED_THREE_QUARTER_INDICATION, MODULATED_MAXIMUM_INDICATION,
    MODE_CHANGE_INDICATION, FAULT_INDICATION, NUMBER_OF_INDICATIONS} indication_t;
    
// LED pattern combinations for each indication
static const LED_combination_t INDICATIONS[NUMBER_OF_INDICATIONS] =
    {{LED_SLOW_BLIPS, LED_OFF}, // ALARM_INDICATION
    {LED_FLASH_1HZ, LED_OFF},   // KICKSTAND_INDICATION
    {LED_ON, LED_OFF},          // UNMODULATED_ON_INDICATION
    {LED_OFF, LED_OFF},         // MODULATED_OFF_INDICATION
    {LED_OFF, LED_ON_BLIPS_1},  // MODULATED_QUARTER_INDICATION
    {LED_OFF, LED_ON_BLIPS_2},  // MODULATED_HALF_INDICATION
    {LED_OFF, LED_ON_BLIPS_3},  // MODULATED_THREE_QUARTER_INDICATION
    {LED_OFF, LED_ON_BLIPS_4},  // MODULATED_MAXIMUM_INDICATION
    {LED_OFF, LED_BLIP_4},      // MODE_CHANGE_INDICATION
    {LED_FLASH_4HZ, LED_OFF}};  // FAULT_INDICATION

// The LED indications for each of the channel 0 power states
static const indication_t CHANNEL_0_INDICATIONS[NUMBER_OF_POWER_MODES][NUMBER_OF_MODULATED_POWER_LEVELS] =
    {{MODULATED_OFF_INDICATION, MODULATED_QUARTER_INDICATION, MODULATED_HALF_INDICATION, MODULATED_THREE_QUARTER_INDICATION, MODULATED_MAXIMUM_INDICATION},
    {UNMODULATED_ON_INDICATION, UNMODULATED_ON_INDICATION, UNMODULATED_ON_INDICATION, UNMODULATED_ON_INDICATION}};


// the function corresponding to the control button and the parameters for interpreting it
#define BUTTON_PRESSED CANASCSwitch()
#define SHORT_PRESS_MINIMUM (50/TIMER_PERIOD)
#define SHORT_PRESS_MAXIMUM (1000/TIMER_PERIOD)
#define VERY_LONG_PRESS (20000/TIMER_PERIOD)
#define BUTTON_DEBOUNCE (50/TIMER_PERIOD) // In order to register the button must be off for at least this long prior to the press


// the state machine's states
typedef enum
{
    STATE_INITIAL=0, // after reset
    STATE_POWER_ON, // fully powered on
    STATE_ALARM_SIMULATION, // pretending to be off apart from pretending to be an alarm
    STATE_POWER_OFF, // pretend to be off while waiting for power to go
    NUMBER_OF_STATES
}states_t;
static states_t state; // state variable

// state machine function table
typedef enum {ENTER_STATE=0, MAINTAIN_STATE} state_action_t;
typedef void (state_function_t)(state_action_t);
typedef state_function_t *state_function_ptr;
static state_function_t InitialState, PowerOnState, AlarmSimulationState, PowerOffState;
static const state_function_ptr STATE_FUNCTIONS[NUMBER_OF_STATES] =
    {&InitialState, &PowerOnState, &AlarmSimulationState, &PowerOffState};


// displays the patterns on the LEDs
static void Indicate(const indication_t indication)
{
    LEDPattern(LED0, INDICATIONS[indication].LED0_pattern);
    LEDPattern(LED1, INDICATIONS[indication].LED1_pattern);
}

// Enters the new state by changing the state variable and invoking the state function of the new state
static void StateTransition(const states_t new_state)
{
    state_function_t *function;
    state = new_state;
    function = STATE_FUNCTIONS[state];
    if (function != NULL)
    {
        function(ENTER_STATE);
    }
}

/* Must be invoked once shortly after power on after the drivers and services are
 * all initialised */
void InitializeApplication(void)
{
    StateTransition(STATE_INITIAL);
}


/* Must be invoked repeatedly at least once per timer tick - just runs the state machine*/
void ApplicationTasks(void)
{
    state_function_t *function = STATE_FUNCTIONS[state];
    if (function != NULL)
    {
        function(MAINTAIN_STATE);
    }
}

static void InitialState(const state_action_t action)
{
    switch (action)
    {
        case ENTER_STATE:
            break;
        case MAINTAIN_STATE:
            StateTransition(STATE_POWER_ON);
            break;
        default:
            break;
    }
}

void PowerOnState(const state_action_t action)
{
    uint16_t now = Timer();
    uint16_t time_since_last_CAN;
    static uint16_t last_CAN_time;
    static uint16_t button_pressed_counter;
    static uint16_t button_debounce_counter;
    static switch_mode_t channel_0_mode;
    static modulated_power_level_t channel_0_modulated_power_level;
    uint16_t eeprom_read_value;

    switch (action)
    {
        case ENTER_STATE:
            PORT_POWER = POWER_PORT_ON;
            eeprom_read_value = DataEERead(MODE_EEPROM_ADDRESS);
            channel_0_mode = (eeprom_read_value < NUMBER_OF_POWER_MODES)?eeprom_read_value:0;
            eeprom_read_value = DataEERead(MODULATION_LEVEL_EEPROM_ADDRESS);
            channel_0_modulated_power_level = (eeprom_read_value < NUMBER_OF_MODULATED_POWER_LEVELS)?eeprom_read_value:0;
            SwitchChipOn();
            SetPWMLevel0(CHANNEL_0_PWM_SETTING[channel_0_mode][channel_0_modulated_power_level],CHANNEL_0_PWM_MODE[channel_0_mode][channel_0_modulated_power_level]);
            SetPWMLevel1(CHANNEL_1_ON, CHANNEL_1_ON_PWM_MODE);
            button_pressed_counter = 0;
            button_debounce_counter = 0;
            last_CAN_time = now;
            break;
        case MAINTAIN_STATE:
            LEDsDim(CANAmbient()); // LED brightness to follow ambient sensor when powered on

            if (CanEcuReceived()) 
            {
                last_CAN_time = now;
            }
            time_since_last_CAN = now - last_CAN_time; // how long ago the last CAN message was received
            // Power off if either the POWER_OFF_DELAY has elapsed or
            // in case of a switch chip fault immediately the ignition is determined to be off
            if ((time_since_last_CAN > POWER_OFF_DELAY)
                || (SwitchChipFault() && (time_since_last_CAN > IGNITION_OFF_DELAY)))
            {
                SetPWMLevel0(CHANNEL_0_OFF, CHANNEL_0_OFF_PWM_MODE);
                SetPWMLevel1(CHANNEL_1_OFF, CHANNEL_1_OFF_PWM_MODE);
                SwitchChipOff();
                DataEEWrite(channel_0_mode, MODE_EEPROM_ADDRESS);
                DataEEWrite(channel_0_modulated_power_level, MODULATION_LEVEL_EEPROM_ADDRESS);
                StateTransition(STATE_ALARM_SIMULATION);
            }
            else if (SwitchChipFault())
            {
                Indicate(FAULT_INDICATION);
            }
            else
            {
#ifdef KICKSTAND_WARNING
                if (CANKickstand() && (time_since_last_CAN < IGNITION_OFF_DELAY))
                {
                    Indicate(KICKSTAND_INDICATION);
                    button_pressed_counter = 0;
                }
                else
#endif
                if (BUTTON_PRESSED)
                {
                    if(button_pressed_counter < UINT16_MAX) ++button_pressed_counter;
                    if ((button_pressed_counter == VERY_LONG_PRESS) && (button_debounce_counter >= BUTTON_DEBOUNCE))
                    {
                        //long button press change the channel power mode
                        if (++channel_0_mode >= NUMBER_OF_POWER_MODES) channel_0_mode = 0;
                        Indicate(MODE_CHANGE_INDICATION);
                        SetPWMLevel0(CHANNEL_0_PWM_SETTING[channel_0_mode][channel_0_modulated_power_level],CHANNEL_0_PWM_MODE[channel_0_mode][channel_0_modulated_power_level]);     
                    }
                }
                else
                {
                    if ((channel_0_mode == MODE_MODULATED) && (button_pressed_counter >= SHORT_PRESS_MINIMUM)
                      && (button_pressed_counter <= SHORT_PRESS_MAXIMUM) && (button_debounce_counter >= BUTTON_DEBOUNCE))
                    {
                       //short button press change modulated power level
                        if (++channel_0_modulated_power_level >= NUMBER_OF_MODULATED_POWER_LEVELS) channel_0_modulated_power_level = 0;
                        SetPWMLevel0(CHANNEL_0_PWM_SETTING[channel_0_mode][channel_0_modulated_power_level],CHANNEL_0_PWM_MODE[channel_0_mode][channel_0_modulated_power_level]);     
                    }
                    if (button_pressed_counter > 0)
                    {
                        button_pressed_counter = 0;
                        button_debounce_counter = 0;
                    }
                    else
                    {
                        if(button_debounce_counter < UINT16_MAX) ++button_debounce_counter;
                    }
                    Indicate(CHANNEL_0_INDICATIONS[channel_0_mode][channel_0_modulated_power_level]);
                }
            }            
            break;
        default:
            break;
    }
}


void AlarmSimulationState(const state_action_t action)
{
    static uint16_t alarm_start_time;
    switch (action)
    {
        case ENTER_STATE:
            Indicate(ALARM_INDICATION);
            LEDsDim(false); // alarm indication should always be bright
            alarm_start_time = Minutes();
            break;
        case MAINTAIN_STATE:
            if (CanEcuReceived())
            {
                StateTransition(STATE_POWER_ON);
            }
            else
#if (ALARM_SIMULATION_TIME > 0)
            if ((Minutes() - alarm_start_time) > ALARM_SIMULATION_TIME)
#endif
                StateTransition(STATE_POWER_OFF);
            break;
        default:
            break;
    }
}


void PowerOffState(const state_action_t action)
{
    switch (action)
    {
        case ENTER_STATE:
            PORT_POWER = POWER_PORT_OFF;
            break;
        case MAINTAIN_STATE:
            if (CanEcuReceived())
            {
                StateTransition(STATE_POWER_ON);
            }
            break;
        default:
            break;
    }
}

