/**********************************************************************
  Filename: lab1_phase2.c
  Date: Oct. 11/13
  File Version: 1.0

  This file implements a simple program that turns LEDs on and off in
  response to a push button input on the Altera DE2 board.  It works
  with the hardware that is built during the Lab Tools Tutorial.  The
  hardware for the button_pio is configured to syncrhonously capture
  both rising and falling edges.  Thus an interrupt is generated when
  a button is pushed and when it is released.  As the corresponding
  LED is toggled on each interrupt it means that when the button is
  pushed the LED is turned on, and when the button is released the
  LED is turned off.

  History
  Ver  Date       Comment
  0.1  Oct 05/13  original prototype
  0.2  Oct 07/13  sample code for tight polling included
  0.3  Oct 08/13  added code for periodic polling
  0.4  Oct 09/13  bugfixes, added code for interrupt handling
  0.5  Oct 10/13  bugfixes, refactoring code
  1.0  Oct 11/13  final refactored version correctly gathering data

**********************************************************************/

void init(int,int);
void finalize(void);
int background(int);

// Libraries required
#include "alt_types.h"  // define types used by Altera code, e.g. alt_u8
#include <stdio.h>
#include <unistd.h>
#include "system.h"  // constants such as the base addresses of any PIOs
                     // defined in your hardware
#include "sys/alt_irq.h"  // required when using interrupts
#include <io.h>

#include "altera_avalon_timer_regs.h"
#include "altera_avalon_pio_regs.h"

int numPulses = 0;  // counts the total number of pulses received from the EGM
int numEdges = 0;  // detects whether a pair of successive rising edges and
				   // falling edges were seen
int previous_pulse = 0;  // stores the state of the previous edge to check for
						 // the occurrence of an edge in the opposite direction

// Function Name: timer_ISR
// Function Purpose: interrupt service request handler for interrupts generated
// 					 by timer_1.
#ifdef TIMER_1_BASE  // only compile this code if there is a timer_1
static void timer_ISR(void* context, alt_u32 id)
{
   // Declare an integer to store the state of the captured edge from
   // PIO_PULSE_BASE. Next, read in the data from PIO_PULSE_BASE.
   int current_pulse = 0;
   current_pulse = IORD(PIO_PULSE_BASE, 0);

   // If the state of the current data sample is different from the state of
   // the previous data sample (from 0 to 1 or from 1 to 0) then an edge has
   // occurred. Update the number of edges that have occurred so far and
   // update the state of the previous pulse with the state of the current
   // pulse.
   if (current_pulse != previous_pulse )
   {
	   numEdges++;
	   previous_pulse = current_pulse;
   }

   // Write the state of the current pulse to the edge capture register of
   // pio_response.
   IOWR_ALTERA_AVALON_PIO_EDGE_CAP(PIO_RESPONSE_BASE, current_pulse);

   // If two edges have occurred, it is interpreted as a full pulse and the
   // number of pulses that have occurred is incremented. The variable storing
   // the number of edges is also reset.
   if (numEdges == 2)
   {
	   numPulses++;
	   numEdges = 0;
   }

   // acknowledge the interrupt by clearing the TO bit in the status register
   IOWR_ALTERA_AVALON_TIMER_STATUS(TIMER_1_BASE, 0x0);
}
#endif

// Function Name: pulse_ISR
// Function Purpose: interrupt service request handler for interrupts generated
// 					 by pio_pulse.
static void pulse_ISR(void* context, alt_u32 id)
{

	// Declare an integer to store the state of the captured edge from
	// PIO_PULSE_BASE. Next, read in the data from PIO_PULSE_BASE.
	int current_pulse = 0;
    current_pulse = IORD(PIO_PULSE_BASE, 0);

	// If the state of the current data sample is different from the state of
    // the previous data sample (from 0 to 1 or from 1 to 0) then an edge has
    // occurred. Update the number of edges that have occurred so far and
    // update the state of the previous pulse with the state of the current
    // pulse.
    if (current_pulse != previous_pulse )
    {
	   numEdges++;
	   previous_pulse = current_pulse;
    }

	// Write the state of the current pulse to the edge capture register of
    // PIO_RESPONSE_BASE.
    IOWR_ALTERA_AVALON_PIO_EDGE_CAP(PIO_RESPONSE_BASE, current_pulse);

	// If two edges have occurred, it is interpreted as a full pulse and the
    // number of pulses that have occurred is incremented. The variable storing
    // the number of edges is also reset.
    if (numEdges == 2)
    {
	   numPulses++;
	   numEdges = 0;
    }

	/* reset the edge capture register of pio_pulse by writing 0x0 to it */
	IOWR_ALTERA_AVALON_PIO_EDGE_CAP(PIO_PULSE_BASE, 0x0);
}

// Function Name: phase2_periodicpolling
// Function Purpose: Performs the necessary steps to set up timer_1 to
//					 periodically poll the EGM for the current pulse at an
//					 interval of 20us. When the interrupts are generated,
// 					 they are handled by the ISR called timer_ISR(). After the
// 					 timer is started and set to run continuously, the init()
// 					 function is called to set up the EGM. An infinite while
// 					 loop is used to run the background task. After 100 pulses
// 					 have been captured, the test results are outputted and
//					 global variables used are reset for a clean testing
// 					 environment.
// Function Parameters:
//			period: the period of each pulse outputted by the EGM (input*81.93us)
//			dutycycle: the duration for which the signal is held high (input*period/16)
//			granularity: the maximum number of task units to complete per each call to
//						 background()
void phase2_periodicpolling(int period, int dutycycle, int granularity)
{
#ifdef TIMER_1_BASE // only compile this code if timer_1 is defined
	  // 32 bit period used for timer
	  alt_u32 timerPeriod;

	  // calculate timer period setting it at
	  // 1s / 50000 = 20us.
	  timerPeriod = TIMER_1_FREQ / 50000;

	  // initialize timer interrupt vector
	  alt_irq_register(TIMER_1_IRQ, (void*)0, timer_ISR);

	  // initialize timer period
	  IOWR(TIMER_1_BASE, 2, (alt_u16)timerPeriod);
	  IOWR(TIMER_1_BASE, 3, (alt_u16)(timerPeriod >> 16));

	  // clear timer interrupt bit in status register
	  IOWR(TIMER_1_BASE, 0, 0x0);

	  // initialize timer control - start timer, run continuously, enable interrupts
	  IOWR(TIMER_1_BASE, 1, 0x7);
#endif

	  // Call init() with the period and dutycycle parameters to set up the
	  // EGM and run the background task with the granularity parameter as
	  // long as the number of pulses is less than 100. After 100 pulses,
	  // call finalize() to output the test results and reset the number of
	  // edges and pulses and the state of the previous_pulse to provide the
	  // next synchronization method test with a clean testing environment.
	  init(period, dutycycle);
	  while(numPulses < 100)
	  {
		  background(granularity);
	  }
	  printf("PERIODIC POLLING - PERIOD: %d\t DUTYCYCLE: %d\t GRANULARITY: %d\t\n", period, dutycycle, granularity);
	  finalize();
	  numPulses = 0;
	  numEdges = 0;
	  previous_pulse = 0;

	  // Make the thread sleep for 0.5s to provide enough time for the
	  // finalize() method to output all of its data to the console.
	  usleep(500000);
}


// Function Name: phase2_tightpolling
// Function Purpose: The init() function is first called to set up the EGM.
// 					 A finite for-loop for 100 iterations is started and set
//					 to capture rising and falling edges while trying to run
//					 the background() task after a rising edge has been
// 					 detected. After 100 pulses have been captured, the test
//					 results are outputted and global variables used are reset
//					 for a clean testing environment.
// Function Parameters:
//			period: the period of each pulse outputted by the EGM (input*81.93us)
//			dutycycle: the duration for which the signal is held high (input*period/16)
//			granularity: the maximum number of task units to complete per each call to
//						 background()
void phase2_tightpolling(int period, int dutycycle, int granularity)
{
	// Call init() with the period and dutycycle parameters to set up the
	// EGM and iterate 100 times through the code in the for-loop to capture
	// 100 pulses. As explained by the TA Bahaedinne Jlassi in the ECE 224 lab
	// sessions, the for-loop is guaranteed to terminate after capturing 100
	// pulses because the bottom of the for-loop can only be reached after
	// capturing both a rising edge and a falling edge.
	int i;
	init(period, dutycycle);
	for(i=0; i<100; i++)
	{
		// Assume that the initial state of the pulse is at the 0 level.
		// While the detected state is still 0, do nothing. Otherwise, write
		// a '1' to the data register of pio_response to indicate a rising
		// edge.
		while(IORD_ALTERA_AVALON_PIO_DATA(PIO_PULSE_BASE) == 0) {}
		IOWR_ALTERA_AVALON_PIO_DATA(PIO_RESPONSE_BASE, 0x1);

		// After the rising edge has been captured, try running the background
		// task to see the tradeoffs in trying to complete background task
		// units and trying to catch pulses from the EGM at the same time.
		background(granularity);

		// Since it is now assumed that the state of the pulse is now at the
		// 1 level, do nothing while the detected state is still 1. Otherwise,
		// write a '0' to the data register of pio_response to indicate a
		// falling edge. Re-iterate through the for-loop to detect the next
		// pulse.
		while(IORD_ALTERA_AVALON_PIO_DATA(PIO_PULSE_BASE) == 1) {}
		IOWR_ALTERA_AVALON_PIO_DATA(PIO_RESPONSE_BASE, 0x0);
	}
	printf("TIGHT POLLING - PERIOD: %d\t DUTYCYCLE: %d\t GRANULARITY: %d\t\n",
			period, dutycycle, granularity);
	finalize();

	// Make the thread sleep for 0.5s to provide enough time for the
	// finalize() method to output all of its data to the console.
	usleep(500000);
}

// Function Name: phase2_interruptsync
// Function Purpose: Performs the necessary steps to set up interrupt
// 					 generation for pio_pulse to generate an interrupt when
//					 an edge is detected from the output signal of the EGM.
//					 When the interrupts are generated, they are handled by
// 					 the ISR called timer_ISR(). After the interrupt setup
//					 procedures are complete, the init() function is called
//					 to set up the EGM. A conditional while loop is used to run
// 					 the background task while 100 pulses have not been
// 					 captured yet. After 100 pulses have been captured,
// 					 the test results are outputted and global variables used
// 					 are reset for a clean testing environment.
// Function Parameters:
//			period: the period of each pulse outputted by the EGM (input*81.93us)
//			dutycycle: the duration for which the signal is held high (input*period/16)
//			granularity: the maximum number of task units to complete per each call to
//						 background()
void phase2_interruptsync(int period, int dutycycle, int granularity)
{
	  /* set up the interrupt vector for pio_pulse, indicating pulse_ISR
	     as the ISR */
	  alt_irq_register( PIO_PULSE_IRQ, (void*)0, pulse_ISR );

	  /* reset the edge capture register by writing 0x0 to it */
	  IOWR_ALTERA_AVALON_PIO_EDGE_CAP(PIO_PULSE_BASE, 0x0);

	  /* set the interrupt request mask and enable interrupts */
	  IOWR_ALTERA_AVALON_PIO_IRQ_MASK(PIO_PULSE_BASE, 0x1);

	  init(period, dutycycle);
	  while(numPulses < 100)
	  {
	  	background(granularity);
	  }
	  printf("INTERRUPT SYNCHRONIZATION - PERIOD: %d\t DUTYCYCLE: %d\t GRANULARITY: %d\t\n", period, dutycycle, granularity);
	  finalize();
	  numPulses = 0;
	  numEdges = 0;
	  previous_pulse = 0;

	  // Make the thread sleep for 0.5s to provide enough time for the
	  // finalize() method to output all of its data to the console.
	  usleep(500000);
}

// Function Name: main
// Function Purpose: To call each of the individual methods for tight polling,
//					 periodic polling, and interrupt synchronization with
//					 different values for the period, duty cycle and
//					 granularity. In the case of periodic polling, the timer
// 					 scaling factor is also modified for some test calls.
int main(void)
{
  /* output initial message */
  phase2_tightpolling(6,8,200);
  phase2_periodicpolling(6,8,200);
  phase2_interruptsync(6,8,200);
  phase2_tightpolling(4,5,200);
  phase2_periodicpolling(4,5,200);
  phase2_interruptsync(4,5,200);
  phase2_tightpolling(3,13,200);
  phase2_periodicpolling(3,13,200);
  phase2_interruptsync(2,13,200);
  phase2_tightpolling(5,2,200);
  phase2_periodicpolling(5,2,200);
  phase2_interruptsync(3,1,200);

  return(0);
}
