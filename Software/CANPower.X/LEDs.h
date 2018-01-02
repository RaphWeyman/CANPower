/* 
 * File:   LEDs.h
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

#ifndef LEDS_H
#define	LEDS_H

#ifdef	__cplusplus
extern "C" {
#endif

    
// The LEDs that this module controls
typedef enum {LED0=0, LED1, NUMBEROFLEDs} LED_t;

// The patterns that this module can output to an LED
typedef enum {LED_OFF=0, LED_ON,
    LED_FLASH_1HZ, LED_FLASH_2HZ, LED_FLASH_4HZ, // continuous flashing
    LED_BLIP, // single one off blip
    LED_BLIPS_1, LED_BLIPS_2, LED_BLIPS_3, LED_BLIPS_4, // a series of blips, pause, repeat continuously
    LED_ON_BLIPS_1, LED_ON_BLIPS_2, LED_ON_BLIPS_3, LED_ON_BLIPS_4, // led on, series of inverse blips, pause, repeat continuously 
    LED_SLOW_BLIPS, // blip every few seconds
    LED_BLIP_4, // series of blips don't repeat
    NUMBEROFLEDPATTERNS} LED_pattern_t;


// set the dimming of the LEDs. True for dim, false for full bright.
void LEDsDim(bool dim);


/* Turn all LEDs off and cancel any patterns that they're outputting */
void AllLEDsOff(void);


/* Output the requested pattern to the requested LED */
void LEDPattern(const LED_t LED, const LED_pattern_t pattern);

/* Must be called once at initialisation time prior to using any of the functionality
  of the LEDs module.
  Assumes that timers and ports are initialised already and that the IO ports are already
  configured as outputs. */
void InitializeLEDs(void);


/* Must be called regularly in order to output the LED patterns. At least as often as the timer
   value can change.*/
void LEDTasks(void);


#ifdef	__cplusplus
}
#endif

#endif	/* LEDS_H */

