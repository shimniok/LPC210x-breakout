LPC2103 Code Examples for IAR Systems KickStart edition of IAR Embedded Workbench for ARM

To make it easier to climb the steep ARM7 learning curve, I've put together a collection of examples that I've tested
on the LPC210x Breakout board running an LPC2103. The examples should easily compile, download and run.

The example code may come from various sources including IAR Systems example code.

After making each project, you'll need to connect your FTDI programmer, then switch the slide switch to PGM to enable
bootloader programming, then use the Flash Magic tool to download the code. The .hex images are found in the Output\Exe
directory of each example. When programming finishes, reset the slider switch and press the reset button and the program
should activate.

Note that an LPC2101 or LPC2102 may not work due to different memory requirements. The configuration files for these
projects is tailored to LPC2103.