#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <inttypes.h>
#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/uart.h"
#include "driverlib/interrupt.h"

#define BUFFER_SIZE     4 
#define GPIO_PA1_U0TX   0x00000401
#define GPIO_PA0_U0RX   0x00000001

uint8_t MemLocal;
uint8_t MemSize[BUFFER_SIZE];
uint32_t FinalLocal;
uint32_t ui32Status;

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

void UART_IntHandler(void){
	
  ui32Status = UARTIntStatus(UART0_BASE,true); //gets the current interrupt status
  UARTIntClear(UART0_BASE,ui32Status); //clears UART interrupt sources
  MemSize[MemLocal%BUFFER_SIZE] = (uint8_t)UARTCharGetNonBlocking( UART0_BASE); //receives a character from the specified port
  MemLocal++; //increases buffer
}

int main(){
  
  uint8_t *CharAux;
  FinalLocal = 0;
  MemLocal = 0;
  int x = 32, y = 65, z = 90;
  
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA); //enables the peripheral
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA)); //determines if a peripheral is ready
  SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);  //enables the peripheral
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_UART0)); //determines if a peripheral is ready

  IntMasterEnable(); //enables the processor interrupt
  UARTConfigSetExpClk(UART0_BASE,(uint32_t)120000000,(uint32_t)115200,(UART_CONFIG_WLEN_8|UART_CONFIG_STOP_ONE|UART_CONFIG_PAR_NONE)); //configure pins for use by the uart peripheral
  UARTFIFODisable(UART0_BASE); //disables the transmit and receive FIFOs
  UARTIntEnable(UART0_BASE,UART_INT_RX|UART_INT_RT); //enables individual UART interrupt sources
  UARTIntRegister(UART0_BASE,UART_IntHandler); //registers an interrupt handler for a UART interrupt
  GPIOPinConfigure(GPIO_PA1_U0TX); //configures the alternate function of a GPIO pin
  GPIOPinConfigure(GPIO_PA0_U0RX);//configures the alternate function of a GPIO pin
  GPIOPinTypeUART(GPIO_PORTA_BASE,(GPIO_PIN_0|GPIO_PIN_1));

  while(1){
 
    if(MemLocal != FinalLocal){
      CharAux = &MemSize[FinalLocal%BUFFER_SIZE]; //saves the message in the buffer
			
      if(((*CharAux)>=(uint8_t)y)&&((*CharAux)<=(uint8_t)z)){
        *CharAux += x;
      }
      if(UARTCharPutNonBlocking(UART0_BASE,MemSize[FinalLocal%BUFFER_SIZE])){ //sends a character to the specified port
        FinalLocal++;//increases the position
      }
    }      
  }
}