/* 
 * File:   timer.c
 * Author: Raph Weyman
 *
 * Created on 04 November 2017, 17:36
 * 
 * Creates a periodic interrupting timebase from Timer 4
 * Must be initialised at startup.
 * 
 */

#include "timer.h"
#include "xc.h"
#include "hardware.h"
#include "interrupts.h"


// free running timer incremented every timer period.
static uint16_t timer;

// free running timer incremented every minute
static uint16_t minute;

// keeps track of the ticks for the minute timer
static uint16_t time_last_minute;


// Returns the current value of the timer.
uint16_t Timer(void)
{
    return timer;
}

// Returns the current minute value. A free counter that increments every minute
// and rolls back through zero on overflow
uint16_t Minutes(void)
{
    return minute;
}


// Invoke once to initialize timer module.
// Timer interrupt is started immediately.
void InitializeTimer(void)
{
    timer = 0;
    minute = 0;
    time_last_minute = 0;
    T4CONbits.TON = 0;
    T4CONbits.TSIDL = 0; // don't stop on idle - the main loop idles and requires a regular interrupt to wake it up again
    T4CONbits.TCKPS = 0b01; //prescaler 8
    TMR4 = 0;
    PR4 = (FCY * TIMER_PERIOD / 8 / 1000);
    _T4IF = 0;
    _T4IP = TIMER_INTERRUPT_PRIORITY;
    _T4IE = 1;
    T4CONbits.TON = 1;
}


// Interrupt service for Timer 4
void __attribute__((interrupt(no_auto_psv))) _T4Interrupt(void)
{
    ++timer;
    if ((timer - time_last_minute) > TICKS_PER_MINUTE)
    {
        ++minute;
        time_last_minute += TICKS_PER_MINUTE;
    }
    _T4IF = 0;
}
