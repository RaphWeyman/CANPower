/* 
 * File:   CAN.h
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
 * Messages to which the module listens are predefined (hard coded). Messages are interpreted
 * under interrupt control with the attributes made available to other modules via the functions
 * defined in this header.
 *
 */

#ifndef CAN_H
#define	CAN_H

#ifdef	__cplusplus
extern "C" {
#endif


/* Must be called once at initialisation time prior to using any functionality
 * of the CAN module. Hardware ports must have been initialised first. */
void InitializeCAN(void);


/* Returns true if a CAN message has been received from the ECU the last time this function was called */
bool CanEcuReceived(void);


/* functions return the state of the various CAN received parameters */
bool CANASCSwitch(void); // true if ASC switch Active
bool CANAmbient(void); // true if dark, false if light
bool CANKickstand(void); // true if kickstand out
uint8_t CANCounter(void); // the value of the counter from the instruments





#ifdef	__cplusplus
}
#endif

#endif	/* CAN_H */

