/*
 * lab1_phase1.c
 *
 *  Created on: 2013-09-19
 *      Author: s244sing
 */


#include <sys/alt_irq.h>
#include <sys/alt_sys_init.h>
#include "alt_types.h"  // define types used by Altera code, e.g. alt_u8
#include <stdio.h>
#include <unistd.h>
#include "system.h"  // constants such as the base addresses of any PIOs
                     // defined in your hardware
#include <io.h>


/*	Method for polling the buttons. */

void get_button_push(int button)
{
	alt_u8 mask; // mask that will be used to select desired bit

	// set up the mask based on the desired button, 0-3
	mask = 0x1;
	mask = mask << button;

	// wait for the desired button to be pushed
	while( (IORD(BUTTON_PIO_BASE, 0) & mask) != 0);

	printf("You pushed button %d\n",button);

	// wait for the desired button to be released
	while( (IORD(BUTTON_PIO_BASE, 0) & mask) == 0);

}


int main(int argc, char* argv[])
{
	get_button_push(0);
	return 0;
}
