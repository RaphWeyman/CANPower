/* 
 * File:   LEDs.c
 * Author: Raph Weyman
 *
 * Created on 05 November 2017, 17:05
 *
 * LED control.
 * Runs timer based sequences or just on and off for each of the LEDs.
 * 
 * Timer module and ports module should be initialised before the LEDs module is
 * initialised.
 * 
 * LEDs are pulse width modulated for brightness control.
 * Timer 2 and Output Compares 2 (LED0) 3 (LED1) are used to do this.
 *
 */
#include <stdlib.h>
#include <stdbool.h>
#include "ports.h"
#include "timer.h"
#include "hardware.h"
#include "LEDs.h"


// values define the PWM control for the LED brightness
// LED outputs are inverted - high output level means fully off. Output compare
// register values must take this into account
#define PWM_PERIOD 2000 // uS - 500Hz
#define PWM_TIMER_COUNT (FCY/64*PWM_PERIOD/1000000) // Timer prescaler is 64
#define PWM_FULL_OFF (PWM_TIMER_COUNT+1) // Output compare register for LED fully off
#define PWM_FULL_ON 0 // Output compare register value for LED fully on
#define PWM_DIM_ON (PWM_TIMER_COUNT*19/20) // Output compare register value for LED dimly on (5%)


// timings for the various LED output patterns
#define FLASH_1HZ_TIME (500 / TIMER_PERIOD) // ms per state
#define FLASH_2HZ_TIME (250/ TIMER_PERIOD) // ms per state
#define FLASH_4HZ_TIME (125 / TIMER_PERIOD) // ms per state
#define BLIP_ON_TIME (20 / TIMER_PERIOD) // ms per blip on state
#define BLIP_OFF_TIME (200/ TIMER_PERIOD) // ms per blip off time for inverse blips
#define INTER_BLIP_TIME (200 / TIMER_PERIOD) // ms between each of a series of blips
#define REPEAT_PAUSE_TIME (1500 / TIMER_PERIOD) // ms pause before repeating a series of blips
#define SLOW_REPEAT_PAUSE_TIME (5000 / TIMER_PERIOD) // ms pause before repeating a slow sequence

// type of data for each segment of an LED output pattern
// most segments have the on/off state of the LED and length of time
// (in timer ticks) that the segment is to be output for.
// time of zero signifies the pattern end marker. End marker state
// value then indicates whether the pattern is to repeat or stop.
// (true = repeat, false = stop with the LED off)
// if the first segment in the sequence is an end marker then the
// state is a simple on/off control for the LED output state.
typedef struct
{
    bool state; // required LED state - true = on
    uint16_t time; // number of timer ticks for this segment of the output pattern
} pattern_segment_t;


// LED output patterns
#define MAX_SEGMENTS 12
static const pattern_segment_t patterns[NUMBEROFLEDPATTERNS][MAX_SEGMENTS]=
{{{false,0}}, // LED_OFF
{{true,0}}, // LED_ON
{{true, FLASH_1HZ_TIME}, {false, FLASH_1HZ_TIME}, {true,0}}, // LED_FLASH_1HZ
{{true, FLASH_2HZ_TIME}, {false, FLASH_2HZ_TIME}, {true,0}}, // LED_FLASH_2HZ
{{true, FLASH_4HZ_TIME}, {false, FLASH_4HZ_TIME}, {true,0}}, // LED_FLASH_4HZ
{{true, BLIP_ON_TIME}, {false, 0}}, // LED_BLIP
{{true, BLIP_ON_TIME}, {false, REPEAT_PAUSE_TIME}, {true, 0}}, // LED_BLIPS_1
{{true, BLIP_ON_TIME}, {false, INTER_BLIP_TIME}, {true, BLIP_ON_TIME}, {false, REPEAT_PAUSE_TIME}, {true, 0}}, // LED_BLIPS_2
{{true, BLIP_ON_TIME}, {false, INTER_BLIP_TIME}, {true, BLIP_ON_TIME}, {false, INTER_BLIP_TIME}, {true, BLIP_ON_TIME}, {false, REPEAT_PAUSE_TIME}, {true, 0}}, // LED_BLIPS_3
{{true, BLIP_ON_TIME}, {false, INTER_BLIP_TIME}, {true, BLIP_ON_TIME}, {false, INTER_BLIP_TIME}, {true, BLIP_ON_TIME}, {false, INTER_BLIP_TIME}, {true, BLIP_ON_TIME}, {false, REPEAT_PAUSE_TIME}, {true, 0}}, // LED_BLIPS_4
{{true, INTER_BLIP_TIME}, {false, BLIP_OFF_TIME}, {true, REPEAT_PAUSE_TIME}, {true, 0}}, // LED_ON_BLIPS_1
{{true, INTER_BLIP_TIME}, {false, BLIP_OFF_TIME}, {true, INTER_BLIP_TIME}, {false, BLIP_OFF_TIME}, {true, REPEAT_PAUSE_TIME}, {true, 0}}, // LED_ON_BLIPS_2
{{true, INTER_BLIP_TIME}, {false, BLIP_OFF_TIME}, {true, INTER_BLIP_TIME}, {false, BLIP_OFF_TIME}, {true, INTER_BLIP_TIME}, {false, BLIP_OFF_TIME}, {true, REPEAT_PAUSE_TIME}, {true, 0}}, // LED_ON_BLIPS_3
{{true, INTER_BLIP_TIME}, {false, BLIP_OFF_TIME}, {true, INTER_BLIP_TIME}, {false, BLIP_OFF_TIME}, {true, INTER_BLIP_TIME}, {false, BLIP_OFF_TIME}, {true, INTER_BLIP_TIME}, {false, BLIP_OFF_TIME}, {true, REPEAT_PAUSE_TIME}, {true, 0}}, // LED_ON_BLIPS_4
{{true, BLIP_ON_TIME}, {false, SLOW_REPEAT_PAUSE_TIME}, {true,0}}, // LED_SLOW_BLIPS
{{true, BLIP_ON_TIME}, {false, INTER_BLIP_TIME}, {true, BLIP_ON_TIME}, {false, INTER_BLIP_TIME}, {true, BLIP_ON_TIME}, {false, INTER_BLIP_TIME}, {true, BLIP_ON_TIME}, {false, INTER_BLIP_TIME}, {false, 0}} // LED_BLIP_4
}; 

        
// control structure used to hold state for each LED
typedef struct
{
    LED_pattern_t led_pattern; // the current pattern being output to the LED
    uint16_t segment; // the current segment of the pattern
    uint16_t timer; // most recent timer value used to derive time based output patterns
}LEDControl_t;

// control structure array for all the LEDs
static LEDControl_t LEDControl[NUMBEROFLEDs];

// true if an on LED should be dimmed
static bool LED_dim;

static void OutputToLEDPort(const LED_t LED, const bool state);


/* Turn all LEDs off and cancel any patterns that they're outputting */
void AllLEDsOff(void)
{
    LED_t i;
    for (i=0; i<NUMBEROFLEDs; ++i)
    {
        LEDPattern(i, LED_OFF);
    }
}


/* Output the requested pattern to the requested LED */
void LEDPattern(const LED_t LED, const LED_pattern_t pattern)
{
    if (pattern != LEDControl[LED].led_pattern) // don't restart a pattern if already being output
    {
        LEDControl[LED].led_pattern = pattern;
        LEDControl[LED].segment = 0;
        LEDControl[LED].timer = Timer();
        OutputToLEDPort(LED, patterns[pattern][0].state);
    }
}


/* Must be called regularly in order to output the LED patterns. At least as often as the timer
   value can change.*/
void LEDTasks(void)
{
    uint16_t time_now = Timer();
    LED_t i;
    LEDControl_t *led_control_ptr;
    LED_pattern_t pattern;
    uint16_t segment;
    uint16_t segment_time;
    for (i=0; i<NUMBEROFLEDs; ++i)
    {
        led_control_ptr = &(LEDControl[i]);
        pattern = led_control_ptr->led_pattern;
        segment = led_control_ptr->segment;
        segment_time = patterns[pattern][segment].time;
        // if the first segment in the pattern is not an end marker (so it's a timed pattern)
        // and the segment time has elapsed then time to output the next segment.
        // otherwise nothing to do
        if ((patterns[pattern][0].time != 0)
          && ((time_now-(led_control_ptr->timer)) > segment_time))
        {
            ++segment;
            bool end_marker = (patterns[pattern][segment].time == 0);
            if (end_marker && !(patterns[pattern][segment].state)) // check if it's an off end marker
            {
                LEDPattern(i, LED_OFF);
            }
            else
            {
                if (end_marker) segment = 0;
                led_control_ptr->segment = segment;
                led_control_ptr->timer += segment_time;
                OutputToLEDPort(i, patterns[pattern][segment].state);
            }
        }
    }
}


/* Must be called once at initialisation time prior to using any of the functionality
  of the LEDs module.
  Assumes that timers and ports are initialised already and that the IO ports are already
  configured as outputs. */
void InitializeLEDs(void)
{
    // Timer 2 and output compares 2 and 3
    T2CONbits.TON = 0;
    T2CONbits.TSIDL = 0; // don't stop on idle
    T2CONbits.TCKPS = 0b10; //prescaler 64
    TMR2 = 0;
    PR2 = PWM_TIMER_COUNT;
    OC2CONbits.OCSIDL = 0; // don't stop on idle
    OC3CONbits.OCSIDL = 0; // don't stop on idle
    OC2CONbits.OCTSEL = 0; // output compare based on timer 0
    OC3CONbits.OCTSEL = 0; // output compare based on timer 0
    OC2R = PWM_FULL_OFF;
    OC3R = PWM_FULL_OFF;
    OC2RS = PWM_FULL_OFF;
    OC3RS = PWM_FULL_OFF;
    OC2CONbits.OCM = 0b110; // PWM mode
    OC3CONbits.OCM = 0b110; // PWM mode
    T2CONbits.TON = 1;
    LED_dim = false;
    AllLEDsOff();
}


/* output the required state of the LED to the actual hardware output port
   state is true for on, false for off*/
static void OutputToLEDPort(const LED_t LED, const bool state)
{
    uint16_t output_value = state?(LED_dim?PWM_DIM_ON:PWM_FULL_ON):PWM_FULL_OFF;
    switch (LED)
    {
        case LED0:
            OC2RS = output_value;
            break;
        case LED1:
            OC3RS = output_value;
            break;
        default:
            break;
    }
}


// set the dimming of the LEDs. True for dim, false for full bright.
void LEDsDim(bool dim)
{
    if (LED_dim != dim)
    {
        LED_dim = dim;
        uint16_t output_on_value = LED_dim?PWM_DIM_ON:PWM_FULL_ON;
        if (OC2RS != PWM_FULL_OFF)
        {
            OC2RS = output_on_value;
        }
        if (OC3RS != PWM_FULL_OFF)
        {
            OC3RS = output_on_value;
        }
    }
}
