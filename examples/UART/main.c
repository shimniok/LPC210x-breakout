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
 *    2. Modified by Michael Shimniok, verified on LPC2103, changed
 *       clock speed, switched to UART0
 *
 * This example project shows how to use the IAR Embedded Workbench for ARM
 * to develop code for the LPC210x Breakout board. 
 * 
 *    http://code.google.com/p/bt-arm-breakout/
 *
 * It shows basic use of the UART0 peripheral which is the UART connected
 * to the FTDI breakout 6-pin header
 *
 * Set your terminal program to 9600,N,8,1
 * 
 * If you see garbage, try resetting the MCU; sometimes the PC and MCU will
 * get out of sync.
 *
 * Jumpers:
 *  DBG       - install in the position closest the MCU (Off)
 *  PGM       - put in PGM position for programming, off to run
 *
 **************************************************************************/
#include <intrinsics.h>
#include <stdio.h>
#include <NXP/iolpc2103.h>

/* Macro Definitions */ 
#define TEMT       (1<<6) 
#define LINE_FEED     0xA    
#define CARRIAGE_RET   0xD 

  
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

  /* Initialize Pin Select Block for Tx and Rx */ 
  PINSEL0=0x5;
  /* Enable FIFO's and reset them */ 
  U0FCR=0x7;   
  /* Set DLAB and word length set to 8bits */ 
  U0LCR=0x83; // 8 bits, 1 stop, no parity, DLAB = 1         
  /* Baud rate set to 9600 */ 
  /* PCLK/16 * BAUD */
  U0FDR = 171; 
  U0DLL = 93;      
  U0DLM = 0;     
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
