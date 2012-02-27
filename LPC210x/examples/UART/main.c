/*************************************************************************
 *
 *    Used with ICCARM and AARM.
 *
 *    (c) Copyright IAR Systems 2006
 *
 *    File name   : main.c
 *    Description : Define main module
 *
 *    History :
 *    1. Date        : 6, January 2006
 *       Author      : Stanimir Bonev
 *       Description : Create
 *
 *  This example project shows how to use the IAR Embedded Workbench for ARM
 * to develop code for the IAR-LPC-2103 Mini evaluation board. It shows
 * basic use of parallel I/O, timer and interrupt controller.
 * It starts by showing different patterns on the LEDs .
 *
 *  The speed of the moving pattern can be controlled by a B1 button
 * and a B2 change patterns.
 *
 * Jumpers:
 *  DBG       - install the jumper
 *  ICSP/RUN  - Off position
 * Buttons:
 *  B1        - Change speed
 *  B2        - Change pattern
 *
 *    $Revision: 47021 $
 **************************************************************************/
#include <intrinsics.h>
#include <stdio.h>
//#include <lpc210x.h>
#include <NXP/iolpc2103.h>

// OSC [Hz]
#define FOSC            12000000UL
// Core clk [Hz]
#define FCCLK           60000000UL
// Per clk [Hz]
#define PCCLK           (FCCLK/2)
// Timer tick per second
#define TICK_PER_SEC    (8UL)
#define TIM_PER_S(Val)  (PCCLK/Val)

/* Macro Definitions */ 
#define TEMT       (1<<6) 
#define LINE_FEED     0xA    
#define CARRIAGE_RET   0xD 

void serialInit(unsigned char dev, unsigned int baud); 
void serialPutc(unsigned char dev, char c);
unsigned char serialEmptyTX(unsigned char dev);
void clockInit();
   
/*************************************************************************
 * Function Name: irq_handler
 * Parameters: none
 *
 * Return: none
 *
 * Description: IRQ handler
 *
 *************************************************************************/
__irq __arm void irq_handler (void)
{
void (*interrupt_function)();
unsigned int vector;

  vector = VICVectAddr;     // Get interrupt vector.
  interrupt_function = (void(*)())vector;
  if(interrupt_function != NULL)
  {
    interrupt_function();  // Call vectored interrupt function.
  }
  else
  {
    VICVectAddr = 0;      // Clear interrupt in VIC.
  }
}

 
/************************* MAIN *************************/ 
 
int main() 
{ 
  int i; 
  char c[]="Hello World!";

  ///////// System Clock Initialization /////////
  // Fosc = 12MHz
  // MSEL=4, PSEL=1
  // PLLCFG=0100100=0x24=36
  // cclk = 60MHz
  // Fcco = 240MHz
  PLLCFG = 0x24;
  // Enable PLL
  PLLCON = 0x1;
  PLLFEED = 0xAA;
  PLLFEED = 0x55;
  // Wait for PLL to achieve lock
  while(!(PLLSTAT & 0x400));
  // Connect PLL
  PLLCON=0x3;
  PLLFEED=0xAA;
  PLLFEED=0x55;

  ///////// Peripheral Clock Initialization /////////
  // Set periphery divider
  // 0x00 => /4
  // 0x01 => /1
  // 0x02 => /2
  APBDIV_bit.APBDIV = 0x02;
  
  // Memory map init flash memory is mapped on 0 address
#ifdef FLASH
  MEMMAP_bit.MAP = 1;
#else
  MEMMAP_bit.MAP = 2;
#endif

  ///////// UART0 Initialization /////////
  
  /* Baud rate set to 9600 */ 
  /* PCLK/16 * BAUD */
  fdr=171;
  dll=93;
  dlm=0;

  /* Initialize Pin Select Block for Tx and Rx */ 
  PINSEL0=0x5;
  /* Enable FIFO's and reset them */ 
  U0FCR=0x7;   
  /* Set DLAB and word length set to 8bits */ 
  U0LCR=0x83; // 8 bits, 1 stop, no parity, DLAB = 1         
  /* Set baud rate */
  U0FDR = fdr; 
  U0DLL = dll;      
  U0DLM = dlm;     
  /* Clear DLAB */     
  U0LCR=0x03; // 8 bits, 1 stop, no parity, DLAB = 0         

  /* Print forever */ 
  while(1) {  
    i=0; 

    /* Keep Transmitting until Null character('\0') is reached */ 
    while(c[i]) { 
      U0THR=c[i]; 
      i++; 
    }     
 
    U0THR=LINE_FEED; 
    U0THR=CARRIAGE_RET; 
   
    /* Wait till U0THR and U0TSR are both empty */ 
    while(!(U0LSR & TEMT));
  }

} 
  
/* These handlers trap instruction fiq, prefetch abort, undefined instruction abort
   and data fetch abort exceptions
*/
__fiq __arm void fiq_handler(void)
{
  while(1);
}

__arm void undef_handler(void)
{
  while(1);
}

__arm void prefetch_handler(void)
{
  while(1);
}

__arm void data_handler(void)
{
  while(1);
}
