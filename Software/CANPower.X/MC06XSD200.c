/* 
 * File:   MC06XSD200.c
 * Author: Raph Weyman
 *
 * Created on 13 November 2017, 18:46
 * 
 * Driver for the NXP MC06XSD200 dual power switch.
 * Driven via the SPI. Expansion port might have another switch on
 * it but this driver only works for the onboard switch which is
 * gated by CS0.
 * 
 * After SwitchChipOn, output PWM levels from 0 (off) to 256 (fully on) can be set
 * for each of the two output channels.
 *
 * Any chip configuration failure or fault condition on either channel causes the entire switch to be turned off. I.e. a fault on
 * either channel will turn off both channels.
 * SwitchFault will then return true. The SwitchChipOn can then be re-invoked.
 *
 * PWM clock is generated from timer 3 and output compare 1
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

#include "MC06XSD200.h"
#include "ports.h"
#include "SPI.h"
#include "hardware.h"
#include "xc.h"

// hardware PWM clock
#define PWM_PERIOD 66 // uS - 15KHz

// requested PWM output levels 0-255
static uint16_t PWM_level_0;
static PWM_mode_t PWM_mode_0;
static uint16_t PWM_level_1;
static PWM_mode_t PWM_mode_1;


// counts up once per MC06XSD200Tasks invocation (assumed to be invoked every timer tick)
// for the software SLOW_PWM mode
static uint8_t software_PWM_counter;


// the state machine
typedef enum
{
    OFF=0, // held in reset
    STARTING0, // about to turn on - state only here to ensure that reset is held long enough in all circumstances
    STARTING1,
    ON, // OUTPUT_WAKE line high - chip just out of reset initialisation not started
    INITIALIZING, // initialisation sequence sent to SPI
    READY, // initialised and ready to have the output state written etc.
    READBACK, // status being read
    PROGRAMMING, // output state being updated
    FAULT // fault condition detected. Switch held in reset
}states_t;
static states_t state;


#define WATCHDOG 0x8000 // SPI command watchdog bit must be toggled regularly
#define PARITY 0x4000 // SPI parity bit - must be even total number of bits set
#define NORMAL_MODE 0x0200 // Normal mode indicated in readback when set

// register configuration values
// each has a value to configure, a mask for the read back checking, and a value for the read back checking
#define GCR_INITIAL_VALUE (0x1810|PARITY) // GCR initial - watchdog disabled, no current sensing, PWM initially disabled
#define GCR_VALUE 0x1980                  // GCR - watchdog enabled, no current sensing, PWM enabled
#define GCR_READBACK_MASK 0x3FFF
#define GCR_READBACK_VALUE (0x1800|NORMAL_MODE|(GCR_VALUE&0x01FF)) 
#define PWM_0_VALUE (0x0400|PARITY)       // PWM_0 - off at level 0
#define PWM_0_READBACK_MASK 0x3E00
#define PWM_0_READBACK_VALUE (0x0800|NORMAL_MODE|(PWM_0_VALUE&0x0000))
#define PWM_1_VALUE (0x2400)              // PWM_1 - off at level 0
#define PWM_1_READBACK_MASK 0x3E00
#define PWM_1_READBACK_VALUE (0x2800|NORMAL_MODE|(PWM_1_VALUE&0x0000))
#define CONFR_0_VALUE (0x08E0)            // CONFR_0 - open load detection disabled, direct control disabled, medium slew
#define CONFR_0_READBACK_MASK 0x3FFF
#define CONFR_0_READBACK_VALUE (0x0C00|NORMAL_MODE|(CONFR_0_VALUE&0x01FF))
#define CONFR_1_VALUE (0x28E0|PARITY)     // CONFR_1 - open load detection disabled, direct control disabled, medium slew
#define CONFR_1_READBACK_MASK 0x3FFF
#define CONFR_1_READBACK_VALUE (0x2C00|NORMAL_MODE|(CONFR_1_VALUE&0x01FF))
#define OCR_0_VALUE (0x1026)              // OCR_0 - external PWM, low current ratios, tOCH1, tOCM1, iOCH2 (23A), iOCM2 (9A), iOCL1 (5A)
#define OCR_0_READBACK_MASK 0x3FFF
#define OCR_0_READBACK_VALUE (0x1000|NORMAL_MODE|(OCR_0_VALUE&0x01FF))
#define OCR_1_VALUE (0x3026|PARITY)       // OCR_1 - external PWM, low current ratios, tOCH1, tOCM1, iOCH2 (23A), iOCM2 (9A), iOCL1 (5A)
#define OCR_1_READBACK_MASK 0x3FFF
#define OCR_1_READBACK_VALUE (0x3000|NORMAL_MODE|(OCR_1_VALUE&0x01FF))
#define RETRYR_0_VALUE (0x1400)           // RETRYR_0 - 16 x 150ms retries 
#define RETRYR_0_READBACK_MASK 0x3FFF
#define RETRYR_0_READBACK_VALUE (0x1400|NORMAL_MODE|(RETRYR_0_VALUE&0x01FF))
#define RETRYR_1_VALUE (0x3400|PARITY)    // RETRYR_1 - 16 x 150ms retries
#define RETRYR_1_READBACK_MASK 0x3FFF
#define RETRYR_1_READBACK_VALUE (0x3400|NORMAL_MODE|(RETRYR_1_VALUE&0x01FF))
// fault and status registers don't have configuration values but are checked after readback
#define STATR_READBACK_MASK 0x3FBC       // any fault condition
#define STATR_READBACK_VALUE (NORMAL_MODE)
#define FAULT_0_READBACK_MASK 0x3F3F     // any fault condition
#define FAULT_0_READBACK_VALUE (0x0400|NORMAL_MODE)
#define FAULT_1_READBACK_MASK 0x3F3F     // any fault condition
#define FAULT_1_READBACK_VALUE (0x2400|NORMAL_MODE)
      
 
// SPI command sequences - maximum SPI_MAX_WORDS each
#define INITIALIZATION_LENGTH 14
static const SPIData_t initialization_sequence[INITIALIZATION_LENGTH] =
{
    GCR_INITIAL_VALUE,
    PWM_0_VALUE,
    PWM_1_VALUE,
    CONFR_0_VALUE,
    CONFR_1_VALUE,
    OCR_0_VALUE,
    OCR_1_VALUE,
    RETRYR_0_VALUE,
    RETRYR_1_VALUE,
    GCR_VALUE,
    // read the fault and status registers so as to clear any initial fault conditions
    0x0001 | PARITY,             // FAULT_0
    0x2001,                      // FAULT_1
    0x0000 ,                     // STATR
    0x0006 | WATCHDOG | PARITY   // dummy GCR read command so that previous value can be read and also a watchdog kick
};

#define READBACK_LENGTH 14
static const SPIData_t readback_sequence[READBACK_LENGTH] =
{
    0x0000 ,                     // STATR
    0x0001 | PARITY,             // FAULT_0
    0x2001,                      // FAULT_1
    0x0002 | PARITY,             // PWMR_0
    0x2002,                      // PWMR_1
    0x0003,                      // CONFR_0
    0x2003 | PARITY,             // CONFR_1
    0x0004 | PARITY,             // OCR_0
    0x2004,                      // OCR_1
    0x0005,                      // RETRYR_0
    0x2005 | PARITY,             // RETRYR_1
    0x0006,                      // GCR
    0x0007 | PARITY,             // DIAGR
    0x0006 | WATCHDOG | PARITY   // dummy GCR read command so that previous value can be read and also a watchdog kick
};

// Register order for the readback
enum {DUMMY=0, STATR, FAULT_0, FAULT_1, PWMR_0, PWMR_1, CONFR_0, CONFR_1, OCR_0, OCR_1, RETRYR_0, RETRYR_1, GCR, DIAGR};

// one to get filled in with the programming commands
#define PROGRAMMING_LENGTH 4
static SPIData_t programming_sequence[PROGRAMMING_LENGTH];

/* returns the supplied word with parity bit set if necessary to keep the total
 * set bits even as required by the MC06XSD200 SPI */
static SPIData_t Parity(const SPIData_t word)
{
    uint8_t count=0;
    SPIData_t shift = word;
    while (shift)
    {
        count+=shift&1;
        shift = shift >> 1;
    }
   return (count&1)?(word^PARITY):word;
}


/* Requests PWM level for switch channel 0.
 * Values 0-256. 0 = fully off, 256 = fully on.
 * Will take effect only when switch chip is on.
 * PWM mode either FAST_PWM (hardware controlled), or SLOW_PWM (software controlled). */
void SetPWMLevel0(const uint16_t level, const PWM_mode_t mode)
{
    PWM_level_0 = (level>PWM_FULL_ON)?PWM_FULL_ON:level;
    PWM_mode_0 = mode;
}


/* Requests PWM level for switch channel 0.
 * Values 0-256. 0 = fully off, 256 = fully on.
 * Will take effect only when switch chip is on.
 * PWM mode either FAST_PWM (hardware controlled), or SLOW_PWM (software controlled). */
void SetPWMLevel1(const uint16_t level, const PWM_mode_t mode)
{
    PWM_level_1 = (level>PWM_FULL_ON)?PWM_FULL_ON:level;
    PWM_mode_1 = mode;
}


/* turns the switch chip off - holds in reset */
void SwitchChipOff(void)
{
    PORT_OUTPUT_WAKE = OUTPUT_WAKE_PORT_INACTIVE;
    // stop the timer and output compare for the PWM clock
    OC1CONbits.OCM = 0b000; // output compare toggles pin
    T3CONbits.TON = 0;
    state = OFF;
}


/* takes the chip out of reset and starts the initialisation
 * sequence.
 */
void SwitchChipOn(void)
{
    if (state == OFF)
    {
        state = STARTING0;
    }
}


/* returns true if a configuration failure or fault condition on either channel has been detected. Chip will
 have been turned off. Can be retried by invoking SwitchChipOn. */
bool SwitchChipFault(void)
{
    return state == FAULT;
}


/* puts the switch chip in reset and initialises the state machine.
 * Starts timer 3 and OC1 for the PWM clock generation
 * SwitchChipOn should be invoked after this if the switch chip is actually to do
 * anything
 */
void InitializeMC06XSD200(void)
{
    software_PWM_counter = 0;
    PWM_level_0 = 0;
    PWM_mode_0 = SLOW_PWM;
    PWM_level_1 = 0;
    PWM_mode_1 = SLOW_PWM;
    SwitchChipOff();

    T3CONbits.TON = 0;
    T3CONbits.TSIDL = 0; // don't stop on idle
    T3CONbits.TCKPS = 0b00; //prescaler 1
    TMR3 = 0;
    PR3 = FCY * PWM_PERIOD / 2 / 1000000;
    OC1CONbits.OCSIDL = 0; // don't stop on idle
    OC1CONbits.OCTSEL = 1; // output compare based on timer 3
    OC1R = FCY * PWM_PERIOD / 4 / 1000000;
}


/* Must be invoked regularly (per timer tick) so as to keep the watchdog serviced
   and the output states up to date etc.
   The startup sequence is driven through some states so as to ensure that the
   reset is driven for as long as it needs to be. Actually it could go 10000 times
   faster than this but it doesn't matter. */
void MC06XSD200Tasks(void)
{
    SPIData_t* readback;
    ++software_PWM_counter;

    switch (state)
    {
        case OFF:
        case FAULT:
            // nothing to do except stay off
            break;
        case STARTING0:
            state = STARTING1;
            break;
        case STARTING1:
            // enable timer and output compare for the PWM clock
            OC1CONbits.OCM = 0b011; // output compare toggles pin
            T3CONbits.TON = 1;
            PORT_OUTPUT_WAKE = OUTPUT_WAKE_PORT_ACTIVE;
            state = ON;
            break;
        case ON:
            if (SPIIdle())
            {
                SPITransfer(initialization_sequence, INITIALIZATION_LENGTH);
                state = INITIALIZING;
            }
            break;
        case INITIALIZING:
            if (SPIIdle())
            {
                SPITransfer(readback_sequence, READBACK_LENGTH);
                state = READBACK;
            }
            break;
        case READY:
            readback = SPIData();
            // check configuration is as it should be - FAULT if not
            if ((readback == NULL)
              || ((readback[STATR] & STATR_READBACK_MASK) != STATR_READBACK_VALUE)
              || ((readback[FAULT_0] & FAULT_0_READBACK_MASK) != FAULT_0_READBACK_VALUE)
              || ((readback[FAULT_1] & FAULT_1_READBACK_MASK) != FAULT_1_READBACK_VALUE)
              || ((readback[PWMR_0] & PWM_0_READBACK_MASK) != PWM_0_READBACK_VALUE)
              || ((readback[PWMR_1] & PWM_1_READBACK_MASK) != PWM_1_READBACK_VALUE)
              || ((readback[CONFR_0] & CONFR_0_READBACK_MASK) != CONFR_0_READBACK_VALUE)
              || ((readback[CONFR_1] & CONFR_1_READBACK_MASK) != CONFR_1_READBACK_VALUE)
              || ((readback[OCR_0] & OCR_0_READBACK_MASK) != OCR_0_READBACK_VALUE)
              || ((readback[OCR_1] & OCR_1_READBACK_MASK) != OCR_1_READBACK_VALUE)
              || ((readback[RETRYR_0] & RETRYR_0_READBACK_MASK) != RETRYR_0_READBACK_VALUE)
              || ((readback[RETRYR_1] & RETRYR_1_READBACK_MASK) != RETRYR_1_READBACK_VALUE)
              || ((readback[GCR] & GCR_READBACK_MASK) != GCR_READBACK_VALUE))
            {
                SwitchChipOff();
                state = FAULT;
            }
            else
            {
                // check whether the set output levels match the requested levels
                // and re-program if different
                uint8_t programming_count = 0;
                // PWM required are exactly as requested if FAST_PWM mode
                // software controlled SLOW_PWM mode puts the output fully on or fully off
                // according to the software_PWM_counter value
                uint16_t required_output_level_0 = ((PWM_mode_0==FAST_PWM)?PWM_level_0:((PWM_level_0>software_PWM_counter)?PWM_FULL_ON:PWM_FULL_OFF));
                uint16_t required_output_level_1 = ((PWM_mode_1==FAST_PWM)?PWM_level_1:((PWM_level_1>software_PWM_counter)?PWM_FULL_ON:PWM_FULL_OFF));
                // MC06XSD200 requires gives 1/256 PWM output when zero is set in the PWM register
                // - to get fully off the "on" bit (bit 8) has to be cleared.
                // Fully on is level 255.
                uint16_t PWM_control_value_0 = 0;
                uint16_t PWM_control_value_1 = 0;
                if (required_output_level_0>0)
                {
                    PWM_control_value_0 = 0x0100|((required_output_level_0 -1)&0x00FF);
                }
                if (required_output_level_1>0)
                {
                    PWM_control_value_1 = 0x0100|((required_output_level_1 -1)&0x00FF);
                }
                if ((readback[PWMR_0] & 0x1ff) != PWM_control_value_0)
                {
                    programming_sequence[programming_count++] = Parity(PWM_0_VALUE | PWM_control_value_0);
                }
                if ((readback[PWMR_1] & 0x1ff) != PWM_control_value_1)
                {
                    programming_sequence[programming_count++] = Parity(PWM_1_VALUE | PWM_control_value_1);
                }
                if (programming_count > 0)
                {
                    SPITransfer(programming_sequence, programming_count);
                    state = PROGRAMMING;
                }
                else // if nothing better to do then readback again so that status can be checked
                {
                    SPITransfer(readback_sequence, READBACK_LENGTH);
                    state = READBACK;
                }
            }
            break;
        case READBACK:
            if (SPIIdle())
            {
                state = READY;
            }
            break;
        case PROGRAMMING:
            if (SPIIdle())
            {
                SPITransfer(readback_sequence, READBACK_LENGTH);
                state = READBACK;
            }
            break;
    }
}
