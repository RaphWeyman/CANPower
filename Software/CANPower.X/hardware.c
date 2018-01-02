/* 
 * File:   hardware.c
 * Author: Raph Weyman
 *
 * Created on 07 November 2017, 17:42
 * 
 * Provides hardware initialisation - particularly the system clock
 * 
 */

#include "hardware.h"
#include "xc.h"


/* must be called early on in the initialisation sequence in order to
 * configure the system clock */
void InitializeHardware(void)
{
    // PLL total multiplier = 8;
    PLLFBD=62; // n x 64
    _PLLPOST=0b01; // n/4
    _PLLPRE=0; // n/2
    // Initiate Clock Switch to Primary Oscillator with PLL (NOSC = 0b011)
    __builtin_write_OSCCONH(0x03);
    __builtin_write_OSCCONL(0x01);
    // Wait for Clock switch to occur
    while (OSCCONbits.COSC != 0b011);
    // Wait for PLL to lock
    while(OSCCONbits.LOCK!=1);
}

