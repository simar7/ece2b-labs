/**********************************************************************
  Filename: lab1_phase1.c
  Date: Oct. 06/13
  File Version: 1.0

  This file implements a program that either turns LED1 on and off or 
  flashes a '0' or a '1' on the seven-segment display in accordance to 
  the values set on switches SW0 to SW7. The switch configuration 
  pattern is flashed on LED1 in response to a push button input on KEY2 
  on the Altera DE2 board. Otherwise, the pattern is flashed on the seven-
  segment display in response to a push button input on KEY3. It works
  with the hardware built during the Lab Tools Tutorial. The hardware for 
  the button_pio is configured to synchronously capture both rising and 
  falling edges.  Thus an interrupt is generated when a button is pushed 
  and when it is released. Since the interrupts are generated twice for 
  each button press, the code in the button ISR takes this into 
  consideration and flashes the pattern on each output only once as 
  requested in the goals for this lab study.

  History
  Ver  Date       Comment
  0.1  Sep 25/13  original prototype using lab tutorial code to detect button
				  presses
  0.2  Sep 27/13  bugfixes in hardware and recreation of lab1_phase1 project
  0.3  Sep 30/13  extended prototype to output specific characters for each 
				  button press
  0.4  Sep 31/13  created temporary C source code file to output pattern on 
				  LED at fixed timer intervals using timer interrupts
  0.5  Oct 02/13  merged code, and added code to read in switch pattern after 
				  button press
  0.6  Oct 03/13  bugfixes, made global variable to hold state of switches and 
				  re-update after each button press
  0.7  Oct 06/13  refactoring code, removing debug statements
  1.0  Oct 07/13  final completed version working correctly on Altera board

**********************************************************************/

// Libraries required
#include "alt_types.h"  // define types used by Altera code, e.g. alt_u8
#include <stdio.h>
#include <unistd.h>
#include "system.h"  // constants such as the base addresses of any PIOs
                     // defined in your hardware
#include "sys/alt_irq.h"  // required when using interrupts
#include <io.h>

// declare global variable of switches state flashed on LEDs, initially all 
// switches should be off
alt_u8 led_switches_state = (alt_u8)0x00;

// declare global variable of switches state flashed on seven-segment display, 
// initially all switches should be off
alt_u8 seven_seg_switches_state = (alt_u8)0x00;

//global variable representing the number of switches left to flash
int led_switch_countdown = 0;  

//global variable representing the number of switches left to flash					
int seven_seg_switch_countdown = 0;

// sevenseg_set_hex method taken from Nios II "count_binary" example 
// provided with the installation of Altera Nios II. The file path is 
// "C:\Software\altera\10.1\nios2eds\examples\software\count_binary\
// count_binary.c"
/*************************************************************************
 * Copyright (c) 2009 Altera Corporation, San Jose, California, USA.      *
 * All rights reserved. All use of this software and documentation is     *
 * subject to the License Agreement located at the end of this file below.*
 *************************************************************************/
#ifdef SEVEN_SEG_PIO_BASE
static void sevenseg_set_hex(int hex)
{
    static alt_u8 segments[16] = {
        0x81, 0xCF, 0x92, 0x86, 0xCC, 0xA4, 0xA0, 0x8F, 0x80, 0x84, /* 0-9 */
        0x88, 0xE0, 0xF2, 0xC2, 0xB0, 0xB8 };                       /* a-f */

    unsigned int data = segments[hex & 15] | (segments[(hex >> 4) & 15] << 8);

    IOWR(SEVEN_SEG_PIO_BASE, 0, data);
}
#endif

// Function Name: timer_ISR
// Function Purpose: Interrupt service request handler for interrupts generated 
// 					 by timer_0. It flashes each switch value on LED1 and/or
//                   the seven-segment display based on the state of the 
//                   switches for each output. Since the timer_ISR is called 
//                   at 1 second intervals in this program, each switch value 
//                   will be flashed on the respective outputs at 1 second 
//                   intervals.
// Function Parameters:
//			context: 
//			id:
#ifdef SEVEN_SEG_PIO_BASE
#ifdef TIMER_0_BASE  // only compile this code if there is a timer_0 defined
static void timer_ISR(void* context, alt_u32 id)
{
   // If there are still switches left to flash on LED1, take the most 
   // significant bit of led_switches_state and update led_state with that 
   // bit value.
   if (led_switch_countdown >= 0)
   {
	   alt_u8 led_state = (alt_u8)0x00;
	   led_state = (led_switches_state & 0x1);
	   
	   /* write current state to LED PIO */
	   IOWR(LED_PIO_BASE, 0, led_state);

	   /* right-shift led_switches_state by 1 to remove flashed switch 
	      and decrement led_switch_countdown */
	   led_switches_state = led_switches_state >> 1;
	   led_switch_countdown--;
   }
   else
   {
	   /* turn off LED1 because there are no more switches to flash */
	   IOWR(LED_PIO_BASE, 0, (alt_u8)0x00);
   }

   // If there are still switches left to flash on the seven-segment display, 
   // take the most significant bit of seven_seg_switches_state and update 
   // seven_seg_state with that bit value.
   if (seven_seg_switch_countdown >= 0)
   {
	   alt_u8 seven_seg_state = (alt_u8)0x00;
	   seven_seg_state = seven_seg_switches_state & 0x1;
	   
	   // use the sevenseg_set_hex() method to output a '1' or '0' depending 
	   // on the bit value set in seven_seg_state.
	   if (seven_seg_state == 0x1)
	   {
		   /* write current state to SEVEN SEG PIO */
		   sevenseg_set_hex(1);
	   }
	   else if (seven_seg_state == 0x0)
	   {
		   /* write current state to SEVEN SEG PIO */
		   sevenseg_set_hex(0);
	   }


	   /* right-shift seven_seg_switches_state by 1 to remove flashed switch 
	      and decrement seven_seg_switch_countdown */
	   seven_seg_switches_state = seven_seg_switches_state >> 1;
	   seven_seg_switch_countdown--;
   }
   else
   {
	   /* turn off seven segment display because there are no switches 
	      to flash */
	   IOWR(SEVEN_SEG_PIO_BASE, 0, (alt_u8)0x00);
   }

   // acknowledge the interrupt by clearing the TO bit in the status register
   IOWR(TIMER_0_BASE, 0, 0x0);
}
#endif
#endif

// Function Name: button_ISR
// Function Purpose: Interrupt service request handler for interrupts generated 
// 					 by button_pio. The button pressed is identified to start 
//                   flashing the switches for LED1 or for the seven-segment 
//                   display. Since the interrupt is generated twice for each 
//                   button press (once for the rising edge and once for the 
//                   falling edge), the switch state is read and the 
//                   number of switches to be outputted is reset to 8 in both 
//                   cases, ensuring that the correct switches will be flashed 
// 					 for the correct amount of time even if there is bouncing 
//					 in the buttons.
// Function Parameters:
//			context: 
//			id:
#ifdef BUTTON_PIO_BASE
#ifdef LED_PIO_BASE
static void button_ISR(void* context, alt_u32 id)
{
   alt_u8 buttons;

   /* get value from edge capture register and mask off all bits except
      the bits corresponding to buttons 1 and 2 */
   buttons = IORD(BUTTON_PIO_BASE, 3) & 0x6;

   if (buttons == 0x2)
   {
	   led_switch_countdown = 8;
	   led_switches_state = IORD(SWITCH_PIO_BASE, 0);
   }
   else if (buttons == 0x4)
   {
	   seven_seg_switch_countdown = 8;
	   seven_seg_switches_state = IORD(SWITCH_PIO_BASE, 0);
   }

   /* reset edge capture register to clear interrupt */
   IOWR(BUTTON_PIO_BASE, 3, 0x0);
}
#endif
#endif

// Function Name: main
// Function Purpose: Main starting point of execution for the program. 
// 					 The interrupts are set up for the push-buttons and the 
//                   timer and enabled. The LEDs and the seven-segment display 
//                   are reset to 0. An infinite while loop is used to keep 
//                   the main program running while pressing the push-buttons 
//                   will go to the button ISR from which the rest of the lab 
//                   study's requested functionality will be demonstrated. 
//                   After each output has been completely flashed, execution 
//                   will return to the infinite while loop until the push-
//                   buttons are pressed again.
int main(void)
{

#ifdef BUTTON_PIO_BASE
  /* initialize the button PIO */

  /* direction is input only */

  /* set up the interrupt vector */
  alt_irq_register( BUTTON_PIO_IRQ, (void*)0, button_ISR );

  /* reset the edge capture register by writing to it (any value will do) */
  IOWR(BUTTON_PIO_BASE, 3, 0x0);

  /* enable interrupts for buttons 1 and 2*/
  IOWR(BUTTON_PIO_BASE, 2, 0x6);

#endif

#ifdef LED_PIO_BASE
  /* initially turn off all LEDs */
  IOWR(LED_PIO_BASE, 0, (alt_u8)0x00);

#endif

#ifdef SEVEN_SEG_PIO_BASE
  /* initially turn off seven segment */
  IOWR(SEVEN_SEG_PIO_BASE, 0, (alt_u8)0x00);
#endif

#ifdef TIMER_0_BASE
  // 32 bit period used for timer
  alt_u32 timerPeriod;  

  // calculate timer period for 1 second
  timerPeriod = TIMER_0_FREQ;

  // initialize timer interrupt vector
  alt_irq_register(TIMER_0_IRQ, (void*)0, timer_ISR);

  // initialize timer period
  IOWR(TIMER_0_BASE, 2, (alt_u16)timerPeriod);
  IOWR(TIMER_0_BASE, 3, (alt_u16)(timerPeriod >> 16));

  // clear timer interrupt bit in status register
  IOWR(TIMER_0_BASE, 0, 0x0);

  // initialize timer control - start timer, run continuously, enable interrupts
  IOWR(TIMER_0_BASE, 1, 0x7);
#endif

  /* output initial message */
  printf("\n\nStart Program.\n");

  while( 1 )
  {
  }

  return(0);
}
/**********************************************************************
  Copyright(c) 2007 C.C.W. Hulls, P.Eng., Students, staff, and faculty
  members at the University of Waterloo are granted a non-exclusive right
  to copy, modify, or use this software for non-commercial teaching,
  learning, and research purposes provided the author(s) are acknowledged.
**********************************************************************/
