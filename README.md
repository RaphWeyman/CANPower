# CANPower
Accessory power controller for BMW CAN bus bikes

Two channel power switch using BMW CAN bus messages to control switch output.
Microchip PIC24 based.

Hardware is a printed circuit board created and modifiable with KiCAD. If no modifications are required then the gerbers zip file
can be used as is to get manufactured. The gerber file names are as required by Elecrow.

The software is packaged as a Microchip PIC IDE project and requires Microchip MPLAB X with the 16 bit PIC compiler installed - all free
from the Microchip website.

The 6 way "debug" header on the PCB connects directly to the the Microchip PICKit 3 debugger/programmer. This or some other programmer is
required in order to program the software to the Microchip MCU.


Functionality may easily be changed by modifying the application.c file.
Here's what the default application does:-
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
 * An alarm simulation is included after the ignition is turned off (and the POWER_OFF_DELAY has elapsed).
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
 * LED is dimmed according to the ambient light sensor CAN bus message.
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

