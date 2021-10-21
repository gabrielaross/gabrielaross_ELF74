//*****************************************************************************
//
// blinky.c - Simple example to blink the on-board LED.
//
// Copyright (c) 2013-2020 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 2.2.0.295 of the EK-TM4C1294XL Firmware Package.
//
//*****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/interrupt.h"

#define SYSTEM_CLOCK_FREQ 10000000
//*****************************************************************************
//
//! \addtogroup example_list
//! <h1>Blinky (blinky)</h1>
//!
//! A very simple example that blinks the on-board LED using direct register
//! access.
//
//*****************************************************************************

//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************

#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
    while(1);
}
#endif

//*****************************************************************************
//
// Blink the on-board LED.
//
//*****************************************************************************

volatile uint32_t IntCounter = 0;

// Interruption Counter
void SysTickIntHandler(void)
{
    IntCounter++;
}
 
int
main(void){
    uint32_t PinRead;
    uint32_t SysTickRead;
    uint32_t ClockPress;   
	
    //Peripherals setup = Button
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ); //enables a peripheral used for the button
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOJ)){ //determines if a peripheral is ready
    }
    
    //Peripherals setup = LED
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION); //enables a peripheral used for the led
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPION)){ //determines if a peripheral is ready
    }
    
    //Sets in and out
    GPIOPinTypeGPIOInput(GPIO_PORTJ_BASE, GPIO_PIN_0); //configures pin for use as GPIO outputs
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_1); //configures pin for use as GPIO outputs
    GPIOPadConfigSet(GPIO_PORTJ_BASE ,GPIO_PIN_0,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU); //get the pad configuration for a pin
   
    //Systick setup and interrupt
    IntMasterEnable(); //enables the processor interrupt
    SysTickPeriodSet(SYSTEM_CLOCK_FREQ);  //sets the period of the counter
    SysTickIntRegister(SysTickIntHandler); //registers an interrupt handler for the systick interrupt
    SysTickEnable(); //enables the counter
    SysTickIntEnable(); //enables the interrupt
    IntCounter = 0;
    
    GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, 0x0); //writes a value to the specified pin
    //(base address, bit-packed representation of the pins, value to write to the pins)
    while(IntCounter<12){//1 second to turn the led on
    }
    
    GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, GPIO_PIN_1); //writes a value to the specified pin
    //(base address, bit-packed representation of the pins, value to write to the pins)
    PinRead = GPIOPinRead(GPIO_PORTJ_BASE,GPIO_PIN_0); //reads the value of the specified pins
    SysTickRead = SysTickValueGet(); //gets the value of the counter
    
    while(PinRead == 1 && IntCounter<36){ //while not pressed, until 3 seconds
      if (IntCounter >= 36){
        printf("There was no interruption under 3 seconds \n");
      }
      PinRead = GPIOPinRead(GPIO_PORTJ_BASE,GPIO_PIN_0); //reads the value of the specified pins 
    }
    
    ClockPress = SysTickValueGet()-SysTickRead+(SYSTEM_CLOCK_FREQ*IntCounter); //gets the value of the counter
    printf("Clocks: %d\n", ClockPress);
 }