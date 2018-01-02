/* 
 * File:   CAN.c
 * Author: Raph Weyman
 *
 * Created on 07 November 2017, 17:08
 * 
 * CAN Module driver.
 * Must be initialised before first use after the ports have been initialised.
 * 
 * Listen-only supported for up to 16 message types (no transmit).
 * DMA channel 3 used to support the transfers.
 * 
 * Messages to which the module listens are defined in CAN_MESSAGE_IDS. Receipt
 * of one of those messages is processed directly in the interrupt routine and so
 * message data format is effectively defined there.
 * 
 * Attributes of interest are interpreted and made available to other modules as
 * dedicated attribute status functions as defined in the header file.
 * 
 * Modifications must ensure that the attributes can be read atomically so that the interrupt
 * cannot mess them up while being read.
 */

#include <stdint.h>
#include <stdbool.h>
#include "CAN.h"
#include "hardware.h"
#include "ports.h"
#include "interrupts.h"
#include "xc.h"


// Messages to which the module listens. Must be exactly 16.
// Unused identifiers must be set to zero.
#define NUMBER_OF_BUFFERS 16
static const uint16_t identifiers[NUMBER_OF_BUFFERS] =
{
    0x10C, // message 0 - ECU
    0x3FF, // message 1 - instruments
    0, // message 2
    0, // message 3
    0, // message 4
    0, // message 5
    0, // message 6
    0, // message 7
    0, // message 8
    0, // message 9
    0, // message 10
    0, // message 11
    0, // message 12
    0, // message 13
    0, // message 14
    0  // message 15
};


// attributes of interest read from the CAN messages
typedef struct
{
    uint8_t counter; // from the instruments - counts up every second
    bool ambient; // ambient light - true if dark, false if light
    bool kickstand; // true if kickstand out
    bool asc_switch; // true if asc switch active
} message_attribute_t;

static const message_attribute_t initial_attribute_values =
{
    0, // from the instruments - counts up every second
    false, // ambient light - true if dark, false if light
    false, // true if kickstand out
    false // true if asc switch active
};

static message_attribute_t message_attributes;

/* functions return the state of the various CAN received parameters */
bool CANASCSwitch(void) {return message_attributes.asc_switch;}
bool CANAmbient(void) {return message_attributes.ambient;}
bool CANKickstand(void) {return message_attributes.kickstand;}
uint8_t CANCounter(void) {return message_attributes.counter;}

// keeps track of when the most recent message was received
static bool can_ecu_received;

/* Returns true if a CAN message has been received from the ECU the last time this function was called */
bool CanEcuReceived(void)
{
    bool return_value = can_ecu_received;
    can_ecu_received = false;
    return return_value;
}


// CAN hardware modes
#define MODE_CONFIGURATION 4
#define MODE_NORMAL 0
#define MODE_DISABLE 1
#define MODE_LOOPBACK 2
#define MODE_LISTEN_ONLY 3
#define MODE_CONFIGURATION 4
#define MODE_LISTEN_ALL_MESSAGES 7



/* rx message buffer allocation */
typedef uint16_t buffer_word_t;
typedef buffer_word_t DMA_BUFFER_t[8];
static DMA_BUFFER_t dma_buffers[NUMBER_OF_BUFFERS] __attribute__((space(dma),aligned(NUMBER_OF_BUFFERS*16)));


// CAN bit timing
// (if anything changes make sure it still works for an integer baud rate prescale within range)
#define CAN_CLOCK 500000 // 500KHz
#define CAN_PROPAGATION_QUANTA 5
#define CAN_PHASE1_QUANTA 5
#define CAN_PHASE2_QUANTA 5
#define CAN_JUMP_WIDTH 3 // must be smaller than the phase2 quanta
#define CAN_QUANTA (CAN_PROPAGATION_QUANTA + CAN_PHASE1_QUANTA + CAN_PHASE2_QUANTA + 1)
#define CAN_BAUD_PRESCALE (FCY / CAN_CLOCK / CAN_QUANTA / 2)


/* Must be called once at initialisation time prior to using any functionality
 * of the CAN module. Hardware ports must have been initialised first.
 * System timer should have been initialized first for the CANReceiveTime to work OK */
void InitializeCAN(void)
{
    message_attributes = initial_attribute_values;
    can_ecu_received = false;

    PORT_CAN_STBY = CAN_ACTIVE;

    // must be in configuration mode before configuring
    _REQOP = MODE_CONFIGURATION;
    while(_OPMODE != MODE_CONFIGURATION);
    C1CTRL1bits.CSIDL = 0;
    _CANCAP = 0;
    _SAM = 1;

    // bit rate
    _SEG2PHTS = 1;
    _BRP = CAN_BAUD_PRESCALE-1;
    _SJW = CAN_JUMP_WIDTH-1;
    _PRSEG = CAN_PROPAGATION_QUANTA-1;
    _SEG1PH = CAN_PHASE1_QUANTA-1;
    _SEG2PH = CAN_PHASE2_QUANTA-1;

    // buffer configuration
    _DMABS=0b100; // 16 buffers
    _FSA = 15; // 16th buffer is a single entry FIFO.
 
    // RX message filters - 16 filters for 16 messages one for each of the 16 Rx buffers
    // all use mask 0 for all bits to match
    _WIN = 0; // to get at buffer TX enables
    _TXEN0 = 0; // all receive buffers
    _TXEN1 = 0;
    _TXEN2 = 0;
    _TXEN3 = 0;
    _TXEN4 = 0;
    _TXEN5 = 0;
    _TXEN6 = 0;
    _TXEN7 = 0;
    _WIN = 1; // register window 1 to get at the mask and filter registers
    C1RXM0SIDbits.SID = 0x7FF; // Mask 0 for all address bits of standard identifier
    C1RXM0SIDbits.MIDE = 1; // Enforce identifier type
    C1FMSKSEL1 = 0; // all filters to use mask 0
    C1FMSKSEL2 = 0; // all filters to use mask 0
    C1RXF0SIDbits.SID = identifiers[0];
    C1RXF1SIDbits.SID = identifiers[1];
    C1RXF2SIDbits.SID = identifiers[2];
    C1RXF3SIDbits.SID = identifiers[3];
    C1RXF4SIDbits.SID = identifiers[4];
    C1RXF5SIDbits.SID = identifiers[5];
    C1RXF6SIDbits.SID = identifiers[6];
    C1RXF7SIDbits.SID = identifiers[7];
    C1RXF8SIDbits.SID = identifiers[8];
    C1RXF9SIDbits.SID = identifiers[9];
    C1RXF10SIDbits.SID = identifiers[10];
    C1RXF11SIDbits.SID = identifiers[11];
    C1RXF12SIDbits.SID = identifiers[12];
    C1RXF13SIDbits.SID = identifiers[13];
    C1RXF14SIDbits.SID = identifiers[14];
    C1RXF15SIDbits.SID = identifiers[15];
    C1RXF0SIDbits.EXIDE = 0; // standard identifier only
    C1RXF1SIDbits.EXIDE = 0;
    C1RXF2SIDbits.EXIDE = 0;
    C1RXF3SIDbits.EXIDE = 0;
    C1RXF4SIDbits.EXIDE = 0;
    C1RXF5SIDbits.EXIDE = 0;
    C1RXF6SIDbits.EXIDE = 0;
    C1RXF7SIDbits.EXIDE = 0;
    C1RXF8SIDbits.EXIDE = 0;
    C1RXF9SIDbits.EXIDE = 0;
    C1RXF10SIDbits.EXIDE = 0;
    C1RXF11SIDbits.EXIDE = 0;
    C1RXF12SIDbits.EXIDE = 0;
    C1RXF13SIDbits.EXIDE = 0;
    C1RXF14SIDbits.EXIDE = 0;
    C1RXF15SIDbits.EXIDE = 0;
    _F0BP = 0; // Put filter 0's message in buffer 0
    _F1BP = 1;
    _F2BP = 2;
    _F3BP = 3;
    _F4BP = 4;
    _F5BP = 5;
    _F6BP = 6;
    _F7BP = 7;
    _F8BP = 8;
    _F9BP = 9;
    _F10BP = 10;
    _F11BP = 11;
    _F12BP = 12;
    _F13BP = 13;
    _F14BP = 14;
    _F15BP = 15;
    _FLTEN0 = (identifiers[0]==0)?0:1;  // enable filters that have a non-zero identifier
    _FLTEN1 = (identifiers[1]==0)?0:1;
    _FLTEN2 = (identifiers[2]==0)?0:1;
    _FLTEN3 = (identifiers[3]==0)?0:1;
    _FLTEN4 = (identifiers[4]==0)?0:1;
    _FLTEN5 = (identifiers[5]==0)?0:1;
    _FLTEN6 = (identifiers[6]==0)?0:1;
    _FLTEN7 = (identifiers[7]==0)?0:1;
    _FLTEN8 = (identifiers[8]==0)?0:1;
    _FLTEN9 = (identifiers[9]==0)?0:1;
    _FLTEN10 = (identifiers[10]==0)?0:1;
    _FLTEN11 = (identifiers[11]==0)?0:1;
    _FLTEN12 = (identifiers[12]==0)?0:1;
    _FLTEN13 = (identifiers[13]==0)?0:1;
    _FLTEN14 = (identifiers[14]==0)?0:1;
    _FLTEN15 = (identifiers[15]==0)?0:1;

    _WIN = 0;
    
    /* Initialize DMA Channel 3 for ECAN RX */
    DMACS0 = 0; // all collision flags clear
    DMA3CON = 0x0020; // peripheral indirect addressed normal word mode]
    DMA3PAD = (uint16_t) &C1RXD;
    DMA3CNT = 7; // data block transfer size 8
    DMA3REQ = 0x0022; // automatic DMA Rx initiation from CAN1 RX
    DMA3STA = __builtin_dmaoffset(&dma_buffers); // buffer address
    DMA3CONbits.CHEN = 1; // DMA enable

    // receiver buffer full interrupt
    _C1IP = CAN_INTERRUPT_PRIORITY;
    _RBIE = 1;
    _C1IE = 1;
    
    // and to the listen-only operational mode
    _REQOP = MODE_LISTEN_ONLY;
    while(_OPMODE != MODE_LISTEN_ONLY);
}


// Interrupt service can receive buffer full
void __attribute__((interrupt(no_auto_psv))) _C1Interrupt(void)
{
    uint16_t vector = _ICODE; // get the code for the interrupt source
    if (vector < 16) // should only be one of the rx buffers - other interrupts are disabled
    {
        buffer_word_t *buffer;
        buffer = dma_buffers[vector];
        if (((buffer[0] & 0x0002) == 0) && ((buffer[2] & 0xf) == 8)) // expect a received message to not be a RTR and to have 8 bytes of data
        {
            // process the message
            // data byte 0 is at buffer[3] low byte
            // data byte 1 is at buffer[3] high byte
            // data byte 2 is at buffer[4] low byte
            // data byte 3 is at buffer[4] high byte
            // data byte 4 is at buffer[5] low byte
            // data byte 5 is at buffer[5] high byte
            // data byte 6 is at buffer[6] low byte
            // data byte 7 is at buffer[7] high byte
            switch(vector)
            {
                case 0: // ECU
                    message_attributes.kickstand = (buffer[5] & 0x0c00)==0x0400?false:true;
                    message_attributes.asc_switch = (buffer[5] & 0x0300)==0x0200?true:false;
                    can_ecu_received = true;
                    break;
                case 1: // instruments
                    message_attributes.counter = buffer[4] & 0xff;
                    message_attributes.ambient = (buffer[3] & 0xc000) == 0x8000?true:false;
                    break;
                default:
                    break;
            }
        }
        C1RXFUL1 &= ~(1<<vector);
    }
    _RBIF = 0;
    _C1IF = 0;
}
