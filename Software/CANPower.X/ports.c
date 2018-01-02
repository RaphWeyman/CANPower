/* 
 * File:   ports.c
 * Author: Raph Weyman
 *
 * Created on 03 November 2017, 19:03
 * 
 * Ports functions for CANPower project
 */

#include <stdint.h>
#include "ports.h"

/* set all ports to their initial states.
   Disconnected ports set to driven etc. */
void InitializePorts(void)
{
    // first drive all unused and output ports to output level 0
    // except for the CS outputs, LED outputs, and CAN standby which are driven high
    LATA = 0x0000; 
    LATB = 0x1400; // for CS0 and CAN standby
    LATC = 0x0018; // for LED0 and LED1
    ODCC = 0x0018; // LED0 and LED1 open drain (for external pull ups)
    TRISA = 0x008C;
    TRISB = 0x4180;
    TRISC = 0x0100;

    // map the selectable pins after unlocking the pin configuration in the OSCCON register
    uint8_t oscconl_value;
    oscconl_value = OSCCONL;
    __builtin_write_OSCCONL(oscconl_value & ~_OSCCON_IOLOCK_MASK); // clear the IOLOCK
   _SDI1R = 24; //SPI1 Data input on RP24
   _RP11R = _RPOUT_SDO1; // SPI1 Data output on RP11
   _RP13R = _RPOUT_SCK1OUT; // SPI1 clock output on RP13
   _RP15R = _RPOUT_OC1; // output compare 1 on RP15 for the PWM clock generation
   _C1RXR = 8; // CAN Rx on RP8
   _RP9R = _RPOUT_C1TX; // CAN Tx on RP9
   _RP20R = _RPOUT_OC2; // output compare 2 on RP20 for the LED0 PWM brightness control
   _RP19R = _RPOUT_OC3; // output compare 3 on RP19 for the LED1 PWM brightness control
   __builtin_write_OSCCONL(oscconl_value | _OSCCON_IOLOCK_MASK); // make sure that the IOLOCK is set
}


