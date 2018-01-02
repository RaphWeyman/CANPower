/*
 * File:   main.c
 * Author: Raph Weyman
 *
 * Created on 03 November 2017, 10:05
 *
 * Basic principle is to execute the Initialisation functions for each module
 * and then repeatedly invoke the Tasks function of each module. However, between each
 * Task invocation the processor will idle - to be woken up by an interrupt. So at least
 * one regular interrupt source must have been set up during module initialisation.
 * Tasks are invoked once per timer tick. Wake-ups for interrupts other than the timer
 * are ignored for the purposes of processing the Tasks.
 */

#include <stdlib.h>
#include <stdbool.h>
#include "xc.h"
#include "ports.h"
#include "timer.h"
#include "LEDs.h"
#include "SPI.h"
#include "CAN.h"
#include "hardware.h"
#include "MC06XSD200.h"
#include "EEPROM.h"
#include "application.h"


// *****************************************************************************
// *****************************************************************************
// ** Initialize
// ** Invoked once at started - calls each module's initialisation
// *****************************************************************************
// *****************************************************************************
void Initialize(void)
{
    InitializeHardware();
    InitializePorts();
    InitializeEEPROM(); // after the hardware but before starting the timer - it could take a while
    InitializeTimer();
    InitializeLEDs();
    InitializeSPI();
    InitializeCAN();
    InitializeMC06XSD200();
    InitializeApplication();
}


// *****************************************************************************
// *****************************************************************************
// ** Tasks
// ** Invoked once per timer tick during normal execution.
// ** Calls each module's tasks function round-robin. Task functions are expected
// ** to return quickly for co-operative multi-tasking.
// *****************************************************************************
// *****************************************************************************
void Tasks(void)
{
    ApplicationTasks();
    LEDTasks();
    MC06XSD200Tasks();
}


// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************

int main(void)
{
    uint16_t last_time, now;
    Initialize();

    // tasks invoked once per timer tick. I.e. when the timer value changes
    last_time = Timer();
    while (true)
    {
        now = Timer();
        if (now != last_time)
        {
            last_time = now;
            Tasks();            
        }
        // watchdog time out is 64ms. Idling resets it. Must be here at least every 64ms
        Idle();
    }

    /* Execution should not come here during normal operation */

    return (EXIT_FAILURE);
}


/*******************************************************************************
 End of File
*/


