########################################################################
#
#                           TimerLED.eww
#
########################################################################

DESCRIPTION
===========

  This example project shows how to use the IAR Embedded Workbench for ARM
  to develop code for Bot Thoughts LPC210x Breakout Board.
  The program shows basic use of GPIO pins, timer and the interrupt controller.

COMPATIBILITY
=============

  The TimerLED example project is compatible with, and works as a
  standalone project, on Bot Thoughts LPC210x breakout board. By default,
  the project is configured to use the Simulator debugger interface.

CONFIGURATION
=============

  1) Make sure that the following jumpers are correctly configured on the
     Bot Thoughts LPC210x breakout board:
	DBG       - install the jumper in the off position
 	PGM       - Off position
        
GETTING STARTED
===============

  1) Start the IAR Embedded Workbench for ARM.

  2) Select File->Open->Workspace...
     Open the TimerLED.eww workspace.

  3) Run the program.

   The TimerLED application is downloaded to the flash memory or to the ram memory 
   on the evaluation board and executed. 
