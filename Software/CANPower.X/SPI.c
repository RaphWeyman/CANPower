/* 
 * File:   SPI.c
 * Author: Raph Weyman
 *
 * Created on 06 November 2017, 20:01
 * 
 * SPI driver.
 * Should be Initialised before first use.
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

#include "SPI.h"
#include "ports.h"
#include "xc.h"
#include "interrupts.h"


static SPIData_t received_data[SPI_MAX_WORDS]; // data received from the slave during transfer
static SPIData_t const* transmit_data; // pointer to data to transmit. NULL if idle.
static uint16_t words_remaining; // number of words still to transfer. Zero if idle.
static uint16_t current_word; // index value of the word currently in transfer


/* Returns true if the SPI is idle and so is able to accept a new SPITransfer.
 * If there has previously been an SPI transfer then the SPIData will return
 * the values received from the slave during that transfer. */
bool SPIIdle(void)
{
    return (transmit_data == NULL);
}


/* Initiates a transfer of length words pointed to by data.
 * Will do nothing if SPI is not idle at the time of invocation.
 * SPIIdle will return true when the transfer is complete at which time
 * SPIData will return a pointer to the values received from the slave */
void SPITransfer(const SPIData_t *const data, const uint16_t length)
{
    if ((transmit_data == NULL) && (length > 0))
    {
        current_word = 0;
        words_remaining = length>SPI_MAX_WORDS?SPI_MAX_WORDS:length;
        transmit_data = data;
        PORT_CS0 = CS0_PORT_ACTIVE; // select slave and then wait 500ns before initiating transfer
        asm(" nop");
        asm(" nop");
        asm(" nop");
        asm(" nop");
        asm(" nop");
        asm(" nop");
        asm(" nop");
        asm(" nop");
        SPI1BUF = transmit_data[current_word];
    }
}


/* Returns a pointer to an array of words received from the SPI slave device
 * during the last transfer or NULL of the SPI is not idle.
 * The number of valid words received will be the same as the length of the
 * transmitted data */
SPIData_t* SPIData(void)
{
    return (transmit_data)?NULL:received_data;
}


/* Must be called once at initialisation time prior to using any of the functionality in the SPI module.
 * Ports must be initialised first. */
void InitializeSPI(void)
{
    words_remaining = 0;
    current_word = 0;
    transmit_data = NULL;
    PORT_CS0 = CS0_PORT_INACTIVE; // make sure that chip is not selected
    _MSTEN = 1;
    _SPISIDL = 0;
    _SPIROV = 0;
    _DISSCK = 0;
    _DISSDO = 0;
    SPI1CON1bits.MODE16 = 1;
    _SMP = 1;
    _CKP = 0;
    _CKE = 0;
    _SSEN = 0;
    _FRMEN = 0; // unframed mode
    _SPIFSD = 0;
    _FRMPOL = 0;
    _FRMDLY = 0;
    _SPRE = 0b111; // 1:1 secondary clock prescale
     _PPRE = 0b01; // 1:16 primary clock prescale - 1MHz
    _SPI1IF = 0;
    _SPI1IP = SPI_INTERRUPT_PRIORITY;
    _SPI1IE = 1;
    _SPIEN = 1;
}


// Interrupt service for SPI transfer complete
void __attribute__((interrupt(no_auto_psv))) _SPI1Interrupt(void)
{
    received_data[current_word++] = SPI1BUF;
    while (_RB13); // wait for clock to go low if it isn't already
    PORT_CS0 = CS0_PORT_INACTIVE; // Slave chip unselect
    if (--words_remaining > 0)
    {
        asm(" nop"); // CS0 must have been inactive for 1uS before taking it active again
        asm(" nop");
        asm(" nop");
        asm(" nop");
        asm(" nop");
        asm(" nop");
        asm(" nop");
        asm(" nop");
        asm(" nop");
        asm(" nop");
        asm(" nop");
        PORT_CS0 = CS0_PORT_ACTIVE; // select slave and then wait 500ns before initiating transfer
        asm(" nop");
        asm(" nop");
        asm(" nop");
        asm(" nop");
        asm(" nop");
        asm(" nop");
        asm(" nop");
        asm(" nop");
        SPI1BUF = transmit_data[current_word];
    }
    else
    {
        transmit_data = NULL;
    }
    _SPI1IF = 0;
}




