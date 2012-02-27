########################################################################
#
#                           Leds.eww
#
# $Revision: 47021 $
#
########################################################################

DESCRIPTION
===========

   This example project shows how to use the IAR Embedded Workbench for ARM
  to develop code for IAR-LPC-2103 Mini evaluation board. It shows 
  basic use of parallel I/O, timer and the interrupt controller.
  It starts by showing different patterns on the LEDs .

   The speed of the moving pattern can be controlled by a B1 button 
  and a B2 change patterns.

COMPATIBILITY
=============

  The Leds example project is compatible with, and works as a
  standalone project, on IAR-LPC-2103 Mini board. By default,
  the project is configured to use the J-Link JTAG interface.

CONFIGURATION
=============

  1) Make sure that the following jumpers are correctly configured on the
     IAR-LPC-2103 Mini board:
	DBG       - install the jumper
 	ICSP/RUN  - Off position
        
GETTING STARTED
===============

  1) Start the IAR Embedded Workbench for ARM.

  2) Select File->Open->Workspace...
     Open the following workspace:

     <installation-root>\arm\examples\NXP\LPC2103\IARMiniDev\
    \Leds\Leds.eww

  3) Run the program.

     The Leds application is downloaded to the flash memory or to the ram memory 
   on the evaluation board and executed. 
  Buttons:
   B1        - Change speed
   B2        - Change pattern
