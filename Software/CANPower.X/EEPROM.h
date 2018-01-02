/* 
 * File:   EEPROM.h
 * Author: Raph Weyman
 *
 * Created on 01 December 2017, 10:43
 * 
 * EEPROM emulation in flash program memory.
 * Copied and modified from Microchip AN1095.
 * Apart from general tidy up and better coding practices, the modifications
 * were to remove multiple banks, better handle errors here were possible (no
 * point in passing errors back up to a layer that's in no position to deal with
 * the resultant mess) and not look for them where a graceful recovery is unlikely anyway.
 * Also made the initialization a bit more likely to recover from a gross corruption.
 * Erroneous page flags removed and write cycles unlimited - no point stopping and producing
 * artificial errors that the higher layers can't handle - nothing to lose by continuing into genuine errors.
 *
 * InitializeEEPROM must be invoked before anything else. It can take a few ms to complete so call before any
 * time critical services have started.
 * DataEERead and DataEEWrite can then be used to read and write data from emulated EEPROM addresses according
 * to the DATA_EE_SIZE. Writes to invalid addresses are ignored and reads from either invalid or unwritten
 * addresses return the ERASED_WORD_VALUE.
 * 
 * Writes cause the CPU to stall - can be for a while if the active flash page needs to be packed into a new one.
 * 
 */

#ifndef EEPROM_H
#define	EEPROM_H

#include "xc.h"

#ifdef	__cplusplus
extern "C" {
#endif

    
// Size of the emulated EEPROM (in words).
// (the highest address is one less than this value)
#define DATA_EE_SIZE 2

// The value of an emulated data word and data byte when unprogrammed
#define ERASED_WORD_VALUE 0xFFFF
#define ERASED_BYTE_VALUE 0xFF


    
void  InitializeEEPROM(void);

uint16_t DataEERead(const uint8_t addr);
void  DataEEWrite(const uint16_t data, const uint8_t addr);


#ifdef	__cplusplus
}
#endif

#endif	/* EEPROM_H */

