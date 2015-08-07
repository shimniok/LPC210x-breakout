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
#include <NXP/iolpc2103.h>

// OSC [Hz]
#define FOSC            12000000UL
// Core clk [Hz]
#define FCCLK           FOSC
// Per clk [Hz]
#define PCCLK           (FOSC/4)
// Timer tick per second
#define TICK_PER_SEC    (8UL)
#define TIM_PER_S(Val)  (PCCLK/Val)

#define MAX_TICK_PER    TIM_PER_S(20)
#define MIN_TICK_PER    TIM_PER_S(5)

// Timer Delta period [ms]
#define DELTA_PER       (50UL)
#define TIM_DPER        ((PCCLK*DELTA_PER)/1000UL)

#define LED_MASK        (1<<8)  // P0.8


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

/*************************************************************************
 * Function Name: VicInit
 * Parameters: none
 *
 * Return: none
 *
 * Description: Init VIC module
 *
 *************************************************************************/
void VicInit (void)
{
  // Assign all interrupt channels to IRQ
  VICIntSelect  =  0;
  // Disable all interrupts
  VICIntEnClear = 0xFFFFFFFF;
  // Clear all software interrupts
  VICSoftIntClear = 0xFFFFFFFF;
  // VIC registers can be accessed in User or privileged mode
  VICProtection = 0;
  // Clear interrupt
  VICVectAddr = 0;
  // Clear address of the Interrupt Service routine (ISR) for non-vectored IRQs.
  VICDefVectAddr = 0;
  // Clear address of the Interrupt Service routine (ISR) for vectored IRQs.
  VICVectAddr0  = 0;
  VICVectAddr1  = 0;
  VICVectAddr2  = 0;
  VICVectAddr3  = 0;
  VICVectAddr4  = 0;
  VICVectAddr5  = 0;
  VICVectAddr6  = 0;
  VICVectAddr7  = 0;
  VICVectAddr8  = 0;
  VICVectAddr9  = 0;
  VICVectAddr10 = 0;
  VICVectAddr11 = 0;
  VICVectAddr12 = 0;
  VICVectAddr13 = 0;
  VICVectAddr14 = 0;
  VICVectAddr15 = 0;
  // Disable all vectored IRQ slots
  VICVectCntl0  = 0;
  VICVectCntl1  = 0;
  VICVectCntl2  = 0;
  VICVectCntl3  = 0;
  VICVectCntl4  = 0;
  VICVectCntl5  = 0;
  VICVectCntl6  = 0;
  VICVectCntl7  = 0;
  VICVectCntl8  = 0;
  VICVectCntl9  = 0;
  VICVectCntl10 = 0;
  VICVectCntl11 = 0;
  VICVectCntl12 = 0;
  VICVectCntl13 = 0;
  VICVectCntl14 = 0;
  VICVectCntl15 = 0;
}

/*************************************************************************
 * Function Name: Timer0Handler
 * Parameters: none
 *
 * Return: none
 *
 * Description: Timer 0 handler
 *
 *************************************************************************/
void Timer0Handler (void)
{
  T0EMR |= 0x002;     // Set MAT1 high for beginning of cycle
  T0IR |= 0x01;       // Clear match 0 interrupt
  VICVectAddr = 0x00; // Dummy write to signal end of interrupt
}

/*************************************************************************
 * Function Name: main
 * Parameters: none
 *
 * Return: none
 *
 * Description: main
 *
 *************************************************************************/
int main(void)
{
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
  while(!(PLLSTAT & 0x400)) ;
  
  // Connect the PLL as the clock source
  PLLCON=0x3;
  PLLFEED=0xAA;
  PLLFEED=0x55;
  
  // Enabling MAM and setting number of clocks used for Flash memory fetch 
  // Per LPC2101/2/3 manual, MAM usage notes, Table 10, says to use 3 cclks for 40-60MHz
  MAMCR=0x2;
  MAMTIM=0x3;

  ///////// Peripheral Clock Initialization /////////
  // Set periphery divider
  // 0x00 => /4
  // 0x01 => /1
  // 0x02 => /2
  APBDIV_bit.APBDIV = 0x00;

  __disable_interrupt();
  // Init VIC
  VicInit();

  // Disable fast IO
  SCS_bit.GPIO0M = 0;
  // Set pins connect to LEDs as outputs
  IODIR = LED_MASK;
  // All LEDs off
  IOCLR = LED_MASK;

  // Init timer
  PINSEL0 |= 0x800;       // Match1 as output
  T0PR = 500;            // Prescaler -- 60MHz /4 = 15MHz /5000 = 3kHz
  T0TCR = 0x02;           // Reset counter and prescale
  T0MCR = 0x03;           // On match, reset counter and generate interrupt
  T0MR0 = 500;            // Set cycle time -- 3kHz /50 = 60Hz
  T0MR1 = 125;            // Set 50% duty cycle
  T0EMR = 0x42;           // On match clear MAT1 (P0.5); MAT1 pin is set high in ISR for first part of cycle
  VICVectAddr4 = (unsigned) Timer0Handler;
  VICVectCntl4 = 0x024;   // set channel
  VICIntEnable |= 0x010; // Enable the interrupt
  T0TCR = 1;              // Enable timer
  
  __enable_interrupt();

  while(1)
  {};
} // main(void)

/* These handlers trap instruction fiq, prefetch abort, undefined instruction abort
   and data fetch abort exceptions
*/
__fiq __arm void fiq_handler(void)
{
  while(1){}
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
