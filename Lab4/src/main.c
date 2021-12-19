#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/uart.h"
#include "inc/hw_memmap.h"
#include "system_TM4C1294.h"
#include "tx_api.h"
#include "utils/uartstdio.h"

#define BUFFER_SIZE 16
#define DEMO_BYTE_POOL_SIZE 9120
#define DEMO_STACK_SIZE 1024
#define DEMO_QUEUE_SIZE 100
#define GPIO_PA0_U0RX 0x00000001
#define GPIO_PA1_U0TX 0x00000401

uint8_t buffer[BUFFER_SIZE];
uint8_t buffer_position;
uint8_t lastposition;

TX_THREAD threadPrincipal;
TX_THREAD threadElevador1;
TX_THREAD threadElevador2;
TX_THREAD threadElevador3;
TX_QUEUE queueElevador1;
TX_QUEUE queueInterna1;
TX_QUEUE queueElevador2;
TX_QUEUE queueInterna2;
TX_QUEUE queueElevador3;
TX_QUEUE queueInterna3;
TX_MUTEX mutex;
TX_BYTE_POOL bytePool;
UCHAR bytePoolMemory[DEMO_BYTE_POOL_SIZE];

void UARTInit(void)
{
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
  while (!SysCtlPeripheralReady(SYSCTL_PERIPH_UART0))
    ;

  GPIOPinConfigure(GPIO_PA0_U0RX);
  GPIOPinConfigure(GPIO_PA1_U0TX);
  GPIOPinTypeUART(GPIO_PORTA_BASE, (GPIO_PIN_0 | GPIO_PIN_1));
  UARTConfigSetExpClk(UART0_BASE, SystemCoreClock, (uint32_t)115200, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
  UARTFIFOEnable(UART0_BASE);

  buffer_position = 0;
  lastposition = 0;
}

int main()
{
  IntMasterEnable();
  SysTickPeriodSet(10000000);
  SysTickIntEnable();
  SysTickEnable();
  UARTInit();
  tx_kernel_enter();
}

void initElevador(char elevador)
{
  UINT status = tx_mutex_get(&mutex, TX_WAIT_FOREVER);

  if (status != TX_SUCCESS)
    return;

  UARTCharPut(UART0_BASE, elevador);
  UARTCharPut(UART0_BASE, 'r');
  UARTCharPut(UART0_BASE, '\n');
  UARTCharPut(UART0_BASE, '\r');

  UARTCharPut(UART0_BASE, elevador);
  UARTCharPut(UART0_BASE, 'f');
  UARTCharPut(UART0_BASE, '\n');
  UARTCharPut(UART0_BASE, '\r');

  status = tx_mutex_put(&mutex);

  if (status != TX_SUCCESS)
    return;
}

void alternarSituacaoPortas(char elevador)
{
  UINT status = tx_mutex_get(&mutex, TX_WAIT_FOREVER);
  if (status != TX_SUCCESS)
    return;

  UARTCharPut(UART0_BASE, elevador);
  UARTCharPut(UART0_BASE, 'a');
  UARTCharPut(UART0_BASE, '\n');
  UARTCharPut(UART0_BASE, '\r');

  status = tx_mutex_put(&mutex);
  if (status != TX_SUCCESS)
    return;

  tx_thread_sleep(1000);

  status = tx_mutex_get(&mutex, TX_WAIT_FOREVER);
  if (status != TX_SUCCESS)
    return;

  UARTCharPut(UART0_BASE, elevador);
  UARTCharPut(UART0_BASE, 'f');
  UARTCharPut(UART0_BASE, '\n');
  UARTCharPut(UART0_BASE, '\r');

  status = tx_mutex_put(&mutex);
  if (status != TX_SUCCESS)
    return;
}

void alternarSituacaoLed(char elevador, char andar, int ligar)
{
  UINT status = tx_mutex_get(&mutex, TX_WAIT_FOREVER);

  if (status != TX_SUCCESS)
    return;

  UARTCharPut(UART0_BASE, elevador);
  if(ligar == 1)
    UARTCharPut(UART0_BASE, 'L');
  else
    UARTCharPut(UART0_BASE, 'D');
  UARTCharPut(UART0_BASE, andar);
  UARTCharPut(UART0_BASE, '\r');

  status = tx_mutex_put(&mutex);

  if (status != TX_SUCCESS)
    return;
}

void moverOuPararElevador(char elevador, int subirOuDescer)
{
  UINT status = tx_mutex_get(&mutex, TX_WAIT_FOREVER);

  if (status != TX_SUCCESS)
    return;

  if (subirOuDescer == 1)
  {
    UARTCharPut(UART0_BASE, elevador);
    UARTCharPut(UART0_BASE, 's');
    UARTCharPut(UART0_BASE, '\n');
    UARTCharPut(UART0_BASE, '\r');
  }
  else if (subirOuDescer == -1)
  {
    UARTCharPut(UART0_BASE, elevador);
    UARTCharPut(UART0_BASE, 'd');
    UARTCharPut(UART0_BASE, '\n');
    UARTCharPut(UART0_BASE, '\r');
  }
  else
  {
    UARTCharPut(UART0_BASE, elevador);
    UARTCharPut(UART0_BASE, 'p');
    UARTCharPut(UART0_BASE, '\n');
    UARTCharPut(UART0_BASE, '\r');
  }

  status = tx_mutex_put(&mutex);

  if (status != TX_SUCCESS)
    return;
}

char consultarAndar(char command, int posX, int posY)
{
    int floor = charToInt(comando[posX]) + charToInt(comando[posY]);

	if (floor >= 0 && floor <= 15){
		char floors[] = "abcdefghijklmnop";
		return floors[floor];
	}
  return floors[0];
}
char *convertCharToString(char c){
    char str1[2] = {c , '\0'};
    char str2[5] = "";
    char *str = malloc(5);
    strcpy(str,str1);
    return str;
}
int indexOf(char * src, char search){
	char * found = strstr( src, convertCharToString(search) );
	int index = -1;
    if (found != NULL)
    {
      index = found - src;
    }
	return index;
}
int charToInt(char value){
	char numbersChar[] = "0123456789";
	int numbers[] = {0,1,2,3,4,5,6,7,8,9};
	int pos = indexOf(numbersChar, value); 
	if (pos > 0){
		return numbers[pos];
	}
	return numbers[0];
}
void elevadorMain(char elevatorPos, ULONG elevador)
{
  char andar = 'a';
  char andarDestino = 'a';
  char comando[16];
  char comandoAtual[16];
  UINT status;
  int subir = -1;
  int tamanho = 0;

  initElevador(elevatorPos);

  while (1)
  {
    memset(comando, 0, sizeof comando);
    status = tx_queue_receive(&queueElevador1, comando, TX_WAIT_FOREVER);
    if (status != TX_SUCCESS)
      break;

	andar = consultarAndar(comando, 1, 2);

    if (comando[1] == 'E' || comando[1] == 'I')
    {
      if(comando[1] == 'I')
        alternarSituacaoLed('e', comando[2], 1);

      tamanho++;
      status = tx_queue_send(&queueInterna1, comando, TX_WAIT_FOREVER);
      if (status != TX_SUCCESS)
        break;
    }
    if (tamanho > 0 && strlen(comandoAtual) == 0)
    {
      status = tx_queue_receive(&queueInterna1, comandoAtual, TX_WAIT_FOREVER);
      if (status != TX_SUCCESS)
        break;
    }
    if (comandoAtual[1] == 'E')
    {
	  andarDestino = consultarAndar(comando, 2, 3);
    }
    else if (comandoAtual[1] == 'I')
    {
      andarDestino = comandoAtual[2];
    }
    if (andarDestino > andar && subir == -1)
    {
      moverOuPararElevador(elevatorPos, 1);
      subir = 1;
    }
    else if (andarDestino < andar && subir == -1)
    {
      moverOuPararElevador(elevatorPos, -1);
      subir = 1;
    }
    if (andarDestino == andar && strlen(comandoAtual) != 0)
    {
      moverOuPararElevador(elevatorPos, 0);
      alternarSituacaoPortas(elevatorPos);
      alternarSituacaoLed(elevatorPos, andarDestino, 0);
      tamanho--;
      memset(comandoAtual, 0, sizeof comandoAtual);
      subir = -1;
    }
  }
}
void elevador1(ULONG elevador){
	elevadorMain('e', elevador);
}
void elevador2(ULONG elevador){
	elevadorMain('c', elevador);
}
void elevador3(ULONG elevador){
	elevadorMain('d', elevador);
}

void mainThread(ULONG msg)
{
  char bufferRequisicoes[16];
  UINT status;
  int pos = 0;
  int entradaProcessada = -1;
  char aux_char;
  while (1)
  {
    while (UARTCharsAvail(UART0_BASE))
    {
      tx_thread_sleep(2);
      aux_char = UARTCharGet(UART0_BASE);
      if (aux_char != '\n' && aux_char != '\r')
      {
        bufferRequisicoes[pos] = aux_char;
        pos++;
        if (aux_char == 'F') //Porta fechada
        {
          memset(bufferRequisicoes, 0, sizeof bufferRequisicoes);
          pos = 0;
        }
      }
      else
      {
        pos = 0;
        entradaProcessada = 1;
      }
    }

    if (entradaProcessada)
    {
      if (bufferRequisicoes[0] == 'e')
      {
        status = tx_queue_send(&queueElevador1, bufferRequisicoes, TX_WAIT_FOREVER);
        if (status != TX_SUCCESS)
          break;
      }
      if (bufferRequisicoes[0] == 'c')
      {
        status = tx_queue_send(&queueElevador2, bufferRequisicoes, TX_WAIT_FOREVER);
        if (status != TX_SUCCESS)
          break;
      }
      if (bufferRequisicoes[0] == 'd')
      {
        status = tx_queue_send(&queueElevador3, bufferRequisicoes, TX_WAIT_FOREVER);
        if (status != TX_SUCCESS)
          break;
      }
      memset(bufferRequisicoes, 0, sizeof bufferRequisicoes);
      entradaProcessada = -1;
    }
  }
}

void tx_application_define(void *first_unused_memory)
{
  CHAR *pointer;

#ifdef TX_ENABLE_EVENT_TRACE
  tx_trace_enable(trace_buffer, sizeof(trace_buffer), 32);
#endif

  tx_byte_pool_create(&bytePool, "byte pool", bytePoolMemory, DEMO_BYTE_POOL_SIZE);

  tx_byte_allocate(&bytePool, (VOID **)&pointer, DEMO_STACK_SIZE, TX_NO_WAIT);
  tx_thread_create(&threadElevador1, "thread elevador 1", elevador1, 2,
                   pointer, DEMO_STACK_SIZE,
                   0, 0, 20, TX_AUTO_START);
  tx_byte_allocate(&bytePool, (VOID **)&pointer, DEMO_STACK_SIZE, TX_NO_WAIT);
  tx_thread_create(&threadElevador2, "thread elevador 2", elevador2, 3,
                   pointer, DEMO_STACK_SIZE,
                   0, 0, 20, TX_AUTO_START);
  tx_byte_allocate(&bytePool, (VOID **)&pointer, DEMO_STACK_SIZE, TX_NO_WAIT);
  tx_thread_create(&threadElevador3, "thread elevador 3", elevador3, 4,
                   pointer, DEMO_STACK_SIZE,
                   0, 0, 20, TX_AUTO_START);
  tx_byte_allocate(&bytePool, (VOID **)&pointer, DEMO_STACK_SIZE, TX_NO_WAIT);
  tx_thread_create(&threadPrincipal, "thread principal", mainThread, 1,
                   pointer, DEMO_STACK_SIZE,
                   0, 0, 20, TX_AUTO_START);

  tx_mutex_create(&mutex, "mutex", TX_NO_INHERIT);

  tx_byte_allocate(&bytePool, (VOID **)&pointer, DEMO_QUEUE_SIZE * sizeof(ULONG), TX_NO_WAIT);
  tx_queue_create(&queueElevador1, "queue elevador 1", TX_1_ULONG, pointer, DEMO_QUEUE_SIZE * sizeof(ULONG));
  tx_byte_allocate(&bytePool, (VOID **)&pointer, DEMO_QUEUE_SIZE * sizeof(ULONG), TX_NO_WAIT);
  tx_queue_create(&queueElevador2, "queue elevador 2", TX_1_ULONG, pointer, DEMO_QUEUE_SIZE * sizeof(ULONG));
  tx_byte_allocate(&bytePool, (VOID **)&pointer, DEMO_QUEUE_SIZE * sizeof(ULONG), TX_NO_WAIT);
  tx_queue_create(&queueElevador3, "queue elevador 3", TX_1_ULONG, pointer, DEMO_QUEUE_SIZE * sizeof(ULONG));
  tx_byte_allocate(&bytePool, (VOID **)&pointer, DEMO_QUEUE_SIZE * sizeof(ULONG), TX_NO_WAIT);
  tx_queue_create(&queueInterna1, "queue interna 1", TX_1_ULONG, pointer, DEMO_QUEUE_SIZE * sizeof(ULONG));
  tx_byte_allocate(&bytePool, (VOID **)&pointer, DEMO_QUEUE_SIZE * sizeof(ULONG), TX_NO_WAIT);
  tx_queue_create(&queueInterna2, "queue interna 2", TX_1_ULONG, pointer, DEMO_QUEUE_SIZE * sizeof(ULONG));
  tx_byte_allocate(&bytePool, (VOID **)&pointer, DEMO_QUEUE_SIZE * sizeof(ULONG), TX_NO_WAIT);
  tx_queue_create(&queueInterna3, "queue interna 3", TX_1_ULONG, pointer, DEMO_QUEUE_SIZE * sizeof(ULONG));

  tx_block_release(pointer);
}
