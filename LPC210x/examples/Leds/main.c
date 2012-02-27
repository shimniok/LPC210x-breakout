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
#include <NXP/iolpc2103.h>

#define LED (1<<12) // P0.12
#define PLOCK 0x400

#define MAX 10000
#define INC 50


/*************************************************************************
 * Function Name: main
 * Parameters: none
 *
 * Return: none
 *
 * Description: Initializes clock PLL, MAM, GPIO for LED, and then runs
 * an infinite loop simulating PWM control of an LED
 *
 *************************************************************************/
int main(void)
{
  int j, duty;
	
  // Initialize the system

  // Clock Initialization
  // Setting Multiplier and Divider values
  PLLCFG=0x24; // 60MHz w/ 12MHz crystal
  PLLFEED=0xAA;
  PLLFEED=0x55;
  
  // Enabling the PLL
  PLLCON=0x1;
  PLLFEED=0xAA;
  PLLFEED=0x55;
  
  // Wait for the PLL to lock to set frequency
  while(!(PLLSTAT & PLOCK)) ;
  
  // Connect the PLL as the clock source
  PLLCON=0x3;
  PLLFEED=0xAA;
  PLLFEED=0x55;
  
  // Enabling MAM and setting number of clocks used for Flash memory fetch 
  // Per LPC2101/2/3 manual, MAM usage notes, Table 10, says to use 3 cclks for 40-60MHz
  MAMCR=0x2;
  MAMTIM=0x3;
  
  // Set io pins for led
  IODIR |= LED;	// led pin is an output, everything else is input after reset
  IOSET =  LED;	// led off
  IOCLR =  LED;	// led on
  
  // endless loop to toggle the LED 
  while (1) {

    // Diminishing glow
    for (duty = MAX; duty > 0; duty -= INC) {
      for (j = 0; j < MAX; j++) {
        if (j < duty)
          IOSET = LED;
        else
          IOCLR = LED;
      }
    }

    // Increasing glow
    for (duty = 0; duty < MAX; duty += INC) {
      for (j = 0; j < MAX; j++) {
        if (j < duty)
          IOSET = LED;
        else
          IOCLR = LED;
      }
    }
    
  }

} // main(void)

/* These handlers trap instruction fiq, prefetch abort, undefined instruction abort
   and data fetch abort exceptions
*/
__irq __arm void irq_handler (void)
{
  while(1);
}

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
