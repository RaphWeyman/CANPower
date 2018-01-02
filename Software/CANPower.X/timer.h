/* 
 * File:   timer.h
 * Author: Raph Weyman
 *
 * Created on 04 November 2017, 17:36
 * 
 * Creates a periodic interrupting timebase from Timer 4
 * Must be initialised at startup.
 * 
 */

#ifndef TIMER_H
#define	TIMER_H

#include <stdint.h>

#ifdef	__cplusplus
extern "C" {
#endif

// timer interrupt period and frequency
// if this is changed make sure that the timer pre-scaler is still appropriate.
#define TIMER_PERIOD 10 //ms
#define TIMER_FREQUENCY (1000 / TIMER_PERIOD) // Hz
#define TICKS_PER_MINUTE (60000 / TIMER_PERIOD)


// Invoke once to initialize timer module.
// Timer interrupt is started immediately.
void InitializeTimer(void);

// Returns the current value of the timer.
// Timer free runs incrementing by 1 at TIMER_FREQUENCY
// and rolling back around to zero on overflow
uint16_t Timer(void);

// Returns the current minute value. A free counter that increments every minute
// and rolls back through zero on overflow
uint16_t Minutes(void);

#ifdef	__cplusplus
}
#endif

#endif	/* TIMER_H */

