/* 
 * File:   hardware.h
 * Author: Raph Weyman
 *
 * Created on 04 November 2017, 17:42
 * 
 * General constants and definitions relating to the hardware configuration
 * and initialisation of the system clock
 */

#ifndef HARDWARE_H
#define	HARDWARE_H

#ifdef	__cplusplus
extern "C" {
#endif


// If the clock frequency is changed check that the system timer and CAN baud rate dividers still
// work as integers etc.
// SPI Chip Select control software delays are also likely to need tweaking.
#define FOSC 32000000 // primary oscillator frequency
#define FCY (FOSC/2)   // system clock frequency

    
/* must be called early on in the initialisation sequence in order to
 * configure the system clock */
void InitializeHardware(void);


#ifdef	__cplusplus
}
#endif

#endif	/* HARDWARE_H */

