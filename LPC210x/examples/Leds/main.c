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
//#define FOSC            14745600UL
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

#define LED_MASK        0x0000FFFF

typedef struct LedPattern_t
{
  unsigned int Pattern;
  struct LedPattern_t const * pNextPattern;
}LedPattern_t;

const LedPattern_t LedPatterns10,LedPatterns11;
const LedPattern_t LedPatterns12,LedPatterns13;

const LedPattern_t LedPatterns20,LedPatterns21;
const LedPattern_t LedPatterns22,LedPatterns23;
const LedPattern_t LedPatterns24,LedPatterns25;

const LedPattern_t LedPatterns30,LedPatterns31;
const LedPattern_t LedPatterns32,LedPatterns33;
const LedPattern_t LedPatterns34,LedPatterns35;

const LedPattern_t LedPatterns10 = {0x00000000,&LedPatterns11};
const LedPattern_t LedPatterns11 = {0x0000003E,&LedPatterns12};
const LedPattern_t LedPatterns12 = {0x00000781,&LedPatterns13};
const LedPattern_t LedPatterns13 = {0x0000F800,&LedPatterns10};

const LedPattern_t LedPatterns20 = {0x00000000,&LedPatterns21};
const LedPattern_t LedPatterns21 = {0x00000882,&LedPatterns22};
const LedPattern_t LedPatterns22 = {0x00001104,&LedPatterns23};
const LedPattern_t LedPatterns23 = {0x00002208,&LedPatterns24};
const LedPattern_t LedPatterns24 = {0x00004410,&LedPatterns25};
const LedPattern_t LedPatterns25 = {0x00000021,&LedPatterns20};

const LedPattern_t LedPatterns30 = {0x00000000,&LedPatterns31};
const LedPattern_t LedPatterns31 = {0x0000003E,&LedPatterns32};
const LedPattern_t LedPatterns32 = {0x000007BF,&LedPatterns33};
const LedPattern_t LedPatterns33 = {0x0000FFBF,&LedPatterns34};
const LedPattern_t LedPatterns34 = {0x000007BF,&LedPatterns35};
const LedPattern_t LedPatterns35 = {0x0000003E,&LedPatterns30};

const LedPattern_t * const pLedPatterns[] =
{
  &LedPatterns10,
  &LedPatterns20,
  &LedPatterns30,
};

int SpeedCounter, PatternInd;
const LedPattern_t * pNextPattern;

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
 * Function Name: KeySpeedIntHandler
 * Parameters: none
 *
 * Return: none
 *
 * Description: Key SPEED interrupt handler
 *
 *************************************************************************/
void KeySpeedIntHandler (void)
{
int T0TC_hold;
  // clear interrupt flag
  EXTINT = 1;
  // Stop timer
  T0TCR_bit.CE = 0;
  // change period
  T0MR0 -= TIM_DPER;
  if(T0MR0 < MAX_TICK_PER)
  {
    T0MR0 = MIN_TICK_PER;
  }
  // check whether the new period is not elapsed
  T0TC_hold = T0TC;     // avoid warning
  if(T0MR0 <= T0TC_hold)
  {
    // reset timer 0
    T0TCR_bit.CR = 1;
  }
  // Enable timer0
  T0TCR = 1;
  VICVectAddr = 0;
}

/*************************************************************************
 * Function Name: KeyChangeIntHandler
 * Parameters: none
 *
 * Return: none
 *
 * Description: Key CHANGE PATTERN interrupt handler
 *
 *************************************************************************/
void KeyChangeIntHandler (void)
{
  // clear interrupt flag
  EXTINT = 4;
  if(++PatternInd >= (sizeof(pLedPatterns)/ sizeof(LedPattern_t *)))
  {
    PatternInd = 0;
  }
  pNextPattern = pLedPatterns[PatternInd];
  VICVectAddr = 0;
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
  // clear interrupt flag
  T0IR_bit.MR0INT = 1;
  // Change pattern
#ifdef FIRST_REV
  IOSET = pNextPattern->Pattern & LED_MASK;
  IOCLR = ~pNextPattern->Pattern & LED_MASK;
#else
  IOSET = ~pNextPattern->Pattern & LED_MASK;
  IOCLR = pNextPattern->Pattern & LED_MASK;
#endif
  pNextPattern = pNextPattern->pNextPattern;
  VICVectAddr = 0;
}

/*************************************************************************
 * Function Name: KeysInit
 * Parameters: none
 *
 * Return: none
 *
 * Description: Init Keys
 *
 *************************************************************************/
void KeysInit (void)
{
  // Set to EXT Interrupt
  PINSEL0_bit.P0_15 = 1;
  PINSEL1_bit.P0_16 = 1;
  // Set ext. interrupt edge to sensitive mode
  EXTMODE = 0x5;
  // Falling Edge
  EXTPOLAR = 0;
  // Set interrupts
  // Assign to IRQ
  VICIntSelect_bit.EINT0 = 0;
  VICIntSelect_bit.EINT2 = 0;
  // Set slots
  VICVectAddr0 = (unsigned int) KeySpeedIntHandler;
  VICVectCntl0_bit.NUMBER = VIC_EINT0;
  VICVectCntl0_bit.ENABLED = 1;
  VICVectAddr1 = (unsigned int) KeyChangeIntHandler;
  VICVectCntl1_bit.NUMBER = VIC_EINT2;
  VICVectCntl1_bit.ENABLED = 1;
  // Clear pending interrupts
  EXTINT = 0x5;
  // Enable interrupts
  VICIntEnable_bit.EINT0 = 1;
  VICIntEnable_bit.EINT2 = 1;
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
  // Disable PLL
  PLLCON = 0;
  // Write Feed
  PLLFEED = 0xAA;
  PLLFEED = 0x55;
  // Set periphery divider /4
  APBDIV_bit.APBDIV  = 0;
  // Memory map init flash memory is mapped on 0 address
#ifdef FLASH
  MEMMAP_bit.MAP = 1;
#else
  MEMMAP_bit.MAP = 2;
#endif

  // Init variables
  PatternInd   = 0;
  pNextPattern = pLedPatterns[0];

  __disable_interrupt();
  // Init VIC
  VicInit();

  // Init GPIO
  PINSEL0 = PINSEL1 = 0;
  // Disable fast IO
  SCS_bit.GPIO0M = 0;
  // Set pins connect to LEDs as outputs
  IODIR = LED_MASK;
  // All LEDs off
  IOCLR = LED_MASK;
  KeysInit();

  // Init timer
  // Reset and stop timer0
  T0TCR = 2;
  // Set timer counters mode - clock by PCLK
  T0CTCR = 0;
  // Set timer prescaler
  T0PR  = 0;
  // Set timer period
  T0MR0 = PCCLK/TICK_PER_SEC;
  // Set match action - interrupt by MACH0 enable, reset counter
  T0MCR = 3;
  // No external action
  T0EMR = 0;
  // Assign to IRQ
  VICIntSelect_bit.TIMER0 = 0;
  // Set interrupt slots
  VICVectAddr3 = (unsigned int) Timer0Handler;
  VICVectCntl3_bit.NUMBER = VIC_TIMER0;
  VICVectCntl3_bit.ENABLED = 1;
  // Timer 0 interrupt enable
  VICIntEnable_bit.TIMER0 = 1;
  // Enable timer0
  T0TCR = 1;

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
