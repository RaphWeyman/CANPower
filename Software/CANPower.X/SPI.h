/* 
 * File:   SPI.h
 * Author: Raph Weyman
 *
 * Created on 06 November 2017, 20:01
 * 
 * SPI driver.
 * Should be Initialized before first use.
 * 
 * Sets up for unframed (3 wire) 16 bit Master mode.
 * CS0 is driven in software (it's not on a re-mappable port).
 * CS1 is left undriven so this module will need some modification if the expansion connector has
 * an SPI device on it.
 * 
 * SPI transfers are initiated by invoking SPITransfer with a pointer to an array of
 * SPIData_t values while the SPI is idle (use SPIIdle to determine).
 * When the transfer is complete SPIIdle will return true and SPIData will return a pointer to
 * an array of SPIData_t values that were received from the SPI slave during the transfer.
 * The maximum number of words that can be transferred in one go is SPI_MAX_WORDS due to the sizing of the
 * receive buffer.
 * 
 * SPI data is transferred under interrupt control so an idling processor will get an
 * interrupt wake up several times including immediately as SPIIdle becomes true.
 * 
 */

#ifndef SPI_H
#define	SPI_H

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#ifdef	__cplusplus
extern "C" {
#endif

// maximum number of words that can be transferred per invocation
#define SPI_MAX_WORDS 16

    
typedef uint16_t SPIData_t;


/* Returns true if the SPI is idle and so is able to accept a new SPITransfer.
 * If there has previously been an SPI transfer then the SPIData will return
 * the values received from the slave during that transfer. */
bool SPIIdle(void);


/* Initiates a transfer of length words pointed to by data.
 * Will do nothing if SPI is not idle at the time of invocation.
 * SPIIdle will return true when the transfer is complete at which time
 * SPIData will return a pointer to the values received from the slave */
void SPITransfer(const SPIData_t *const data, const uint16_t length);


/* Returns a pointer to an array of words received from the SPI slave device
 * during the last transfer or NULL of the SPI is not idle.
 * The number of valid words received will be the same as the length of the
 * transmitted data */
SPIData_t* SPIData(void);


/* Must be called once at initialisation time prior to using any of the functionality in the SPI module.
 * Ports must be initialised first. */
void InitializeSPI(void);

#ifdef	__cplusplus
}
#endif

#endif	/* SPI_H */

