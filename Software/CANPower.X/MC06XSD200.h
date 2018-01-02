/* 
 * File:   MC06XSD200.h
 * Author: Raph Weyman
 *
 * Created on 13 November 2017, 18:46
 * 
 * Driver for the NXP MC06XSD200 dual power switch.
 * Driven via the SPI gated by CS0.
 *
 * After SwitchChipOn, output PWM levels from 0 (off) to 256 (fully on) can be set
 * for each of the two output channels.
 *
 * Any chip configuration failure or fault condition on either channel causes the entire switch to be turned off. I.e. a fault on
 * either channel will turn off both channels.
 * SwitchFault will then return true. The SwitchChipOn can then be re-invoked.
 *
 * PWM clock is generated from timer 3 and output compare 1.
 * PWM clock frequency is 15KHz for a switch output PWM frequency of 59Hz.
 *
 * Each channel can be run in SLOW_PWM or FAST_PWM mode. FAST_PWM is hardware controlled output at
 * the PWM frequency.
 * SLOW_PWM is software controlled. The hardware PWM output is either fully on or fully off but cycled slowly
 * under software control over 256 timer ticks (2.56 seconds at 10ms timer ticks).
 * 
 * SPI and ports and the system timer must have been initialised prior to
 * initialising this module.
 */

#ifndef MC06XSD200_H
#define	MC06XSD200_H

#include <stdint.h>
#include <stdbool.h>


#ifdef	__cplusplus
extern "C" {
#endif

#define PWM_FULL_ON 256 // PWM value for fully on (unmodulated)
#define PWM_FULL_OFF 0  // PWM value for fully off (unmodulated)

typedef enum {SLOW_PWM, FAST_PWM} PWM_mode_t;

/* Requests PWM level for switch channel 0 and 1 respectively.
 * Values 0-256. 0 = fully off, 256 = fully on.
 * Will take effect only when switch chip is on.
 * PWM mode either FAST_PWM (hardware controlled), or SLOW_PWM (software controlled). */
void SetPWMLevel0(const uint16_t level, const PWM_mode_t const mode);
void SetPWMLevel1(const uint16_t level, const PWM_mode_t mode);


/* turns the switch chip off - holds in reset */
void SwitchChipOff(void);


/* takes the chip out of reset and starts the initialisation
   sequence */
void SwitchChipOn(void);

/* returns true if a configuration failure or fault condition on either channel has been detected. Chip will
 have been turned off. Can be retried by invoking SwitchChipOn. */
bool SwitchChipFault(void);

/* Must be invoked regularly (per timer tick) so as to keep the watchdog serviced
   and the output states up to date etc.*/
void MC06XSD200Tasks(void);

/* puts the switch chip in reset and initialises the state machine.
 * SwitchChipOn should be invoked after this if the switch chip is actually to do
 * anything */
void InitializeMC06XSD200(void);


#ifdef	__cplusplus
}
#endif

#endif	/* MC06XSD200_H */

