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

#include <math.h> // pow function
#include <stdlib.h>

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

void sendMessage(int recognizedLine, int direction, int leftOdometry, int rightOdometry){
	char text[20] = "";
	int averageOdometry = ((leftOdometry / 27) + (rightOdometry / 27)) / 2;
	int byte = averageOdometry + pow(2,6)*direction + pow(2,7)*recognizedLine;
	unsigned char message = byte & 0xFF;
	while(1){
		if(!uart0_txfull()){
			uart0_putchar(message);
			// prints the char "to send" on the display
			sprintf(text, "Gesendet: %x", message);
			gfx_move(0, 20);
			gfx_print_text(text);
		}
		delay(500);
		if (!uart0_rxempty()){	// wait for answer
			break;
		}
	}
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

	sendMessage(0, 1, 27, 27);
	delay(4000);
	sendMessage(0, 1, 27, 27);
	delay(4000);
	sendMessage(0, 0, 27, 27);
	delay(4000);
	sendMessage(0, 1, 27, 27);
	delay(4000);
	sendMessage(0, 0, 27, 27);
	delay(4000);
	sendMessage(0, 0, 54, 54);
	delay(4000);
	sendMessage(0, 1, 27, 27);
	delay(4000);
	sendMessage(0, 1, 81, 81);
	delay(4000);
	sendMessage(0, 1, 27, 27);
	delay(4000);
	sendMessage(0, 0, 27, 27);
	delay(4000);
	sendMessage(0, 1, 27, 27);
	delay(4000);
	sendMessage(0, 1, 27, 27);
	delay(4000);
	sendMessage(0, 0, 27, 27);
	delay(4000);
	sendMessage(0, 1, 27, 27);
	delay(4000);
	sendMessage(0, 0, 27, 27);
	delay(4000);
	sendMessage(1, 1, 54, 54);
	delay(4000);
	return 0;
}


