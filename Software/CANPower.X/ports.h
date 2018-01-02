/* 
 * File:   ports.h
 * Author: Raph Weyman
 *
 * Created on 03 November 2017, 19:03
 * 
 * Ports bit definitions and functions for CANPower project
 */

#ifndef PORTS_H
#define	PORTS_H

#include "xc.h"

#ifdef	__cplusplus
extern "C" {
#endif

// LED output port positions - but normally use the PWM control instead
#define PORT_LED0 _LATC4
#define PORT_LED1 _LATC3

// SPI Chip Select output port positions
#define PORT_OUTPUT_WAKE _LATA10
#define OUTPUT_WAKE_PORT_ACTIVE 1
#define OUTPUT_WAKE_PORT_INACTIVE 0
#define PORT_CS0 _LATB12
#define CS0_PORT_ACTIVE 0
#define CS0_PORT_INACTIVE 1

// CAN device output port positions
#define PORT_CAN_STBY _LATB10
#define CAN_ACTIVE 0
#define CAN_INACTIVE 1
    
// Power supply control latch
#define PORT_POWER _LATC5
#define POWER_PORT_ON 1
#define POWER_PORT_OFF 0


/* set all ports to their initial states.
   Disconnected and output ports set to driven etc. */
void InitializePorts(void);


#ifdef	__cplusplus
}
#endif

#endif	/* PORTS_H */

