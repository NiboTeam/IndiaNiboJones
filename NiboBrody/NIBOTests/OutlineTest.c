/**
 *\author Felix Hahn
 *\date 04.10.2014
 *\version 1.0
 *\brief This Program can be used to test the XBEE communication between to XBEE modules.
 *\It receives the incoming data on the UART0-Port(This is where the XBEE-Module is connected to on your Nibo)
 *\of the Nibo and prints it on the display of the Nibo. Afterwards the programm sends the received chars back to the other module.
 *\ Works with 9600 bit/s with 8N1 (see e.g. http://universal_lexikon.deacademic.com/201427/8N1) !
 */



// libraries for standard functions of the Nibo2
#include <nibo/niboconfig.h>
#include <nibo/iodefs.h>
#include <nibo/bot.h>

// libary for the use of the display
#include <nibo/display.h>

// libary for the use of the graphical functions
#include <nibo/gfx.h>

// libary for standard input and output operations of the Nibo2
#include <stdio.h>

// includes interrupts
#include <stdint.h>

// includes header file for the use of the UART1-Port
#include "uart0.h"

// include delay operations
#include <nibo/delay.h>


// includes SPI communication library
#include <nibo/spi.h>

// includes microcontroller hardware interrupts
#include <avr/interrupt.h>

/**
 * Function which initialize the UART0 Port of the Nibo and sets the baudrate of the port
 */
void initUART0(){
	uart0_set_baudrate(9600);
	uart0_enable();
}

/**
 * Main function of the helloXBee.c file.
 * Receives and sends chars using the UART1-Port of the Nibo.
 */
int main(){


	// initialize the robot
	bot_init();

	// initialize spi-port
	spi_init();

	// initialize the display and the graphical operations
	display_init();
	gfx_init();

	// buffer for the chars, is needed to print the chars on the display
	char text[20] = "";

	// calls the function which initialize the UART0-Port
	initUART0();

	// declare and initialize a variable for storing received characters
	char x=0;

	// prints text on the Display
	sprintf(text, "Hallo XBEE");
	gfx_move(0, 0);
	gfx_print_text(text);
	int i = 0;
	// Operation loop
	while (1)
	{
		// if the receive buffer is not empty ->
		if (!uart0_rxempty())
		{
			i++;
			// -> get char out of the buffer
			x=uart0_getchar();
			// prints the received char on the display
			sprintf(text, "Nachrichten: %d", i);
			gfx_move(0, 15);
			gfx_print_text(text);
			if(!uart0_txfull())
			{
				uart0_putchar("1");
			}

		}
		// Delay for the operation loop
		_delay_ms(10);

	}

	return 0;

}


