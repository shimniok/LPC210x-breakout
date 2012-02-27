########################################################################
# Leds.eww
# Originally adapted from IAR Systems example code
########################################################################

DESCRIPTION
===========

  This example project shows how to use the IAR Embedded Workbench for ARM
  to develop code for the LPC210x Breakout board:

       http://code.google.com/p/bt-arm-breakout

  The project demonstrates basic use of GPIOs.  The project uses looping
  to simulate PWM control of an LED on P0.12

COMPATIBILITY
=============

  The Leds example project is compatible with, and works as a standalone project,
  on the LPC210x Breakout board but should work equally well on many LPC2103
  boards.

  The project is configured to use the Simulator.

CONFIGURATION
=============

  1) Make sure that the following are correctly configured on the LP210x board:
	DBG  - disable by moving the jumper to the position closest to the MCU
 	PGM  - Switch to PGM position for programming and off for run
        
GETTING STARTED
===============

  1) Start the IAR Embedded Workbench for ARM.

  2) Select File->Open->Workspace...
     Open the Leds.eww workspace

  3) Make the program

  4) Set the PGM switch to PGM

  5) Use Flash Magic to download the .hex file found in <workspace directory>Output\Exe

  6) Switch PGM to off and press the reset button

