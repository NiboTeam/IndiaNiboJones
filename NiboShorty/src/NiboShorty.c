/**
 * @file NiboShorty.c
 * @brief This program plays a sound on the passive Nibo if the active Nibo is
 * hitting a black line and stops playing the sound if the active Nibo passes
 * a second black line
 * @author Philip Mitzlaff
 * @date 13.12.2018
 * @version 1.0
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

#include <nibo/sound.h>
#include <avr/interrupt.h>
#include "n2sound.h"
#include "indigraphic.xbm"

/**
 * Function which initialize the UART0 Port of the Nibo and sets the baudrate of the port
 */
void initUART0() {
	uart0_set_baudrate(9600);
	uart0_enable();
}

/**
 * function that plays the Indiana Jones title song
 */
void playIndi() {
	//note timings, tempo = 140 bpm
	int l8 = 214;
	int l16 = 107;
	cli();
	//Atomarer Zustand: damit die Toene korrekt abgespielt werden.
	//Lied wird komplett abgespielt und dann unterbrochen werden.

	//Indiana Jones -Soundtrack
	tone(E4, l16);
	_delay_ms(l16);
	tone(F4, l16);
	tone(G4, l16);
	_delay_ms(l8);
	tone(C5, l16);
	_delay_ms(l8);
	_delay_ms(l8);
	_delay_ms(l8);
	_delay_ms(l8);
	tone(D4, l16);
	_delay_ms(l8);
	tone(E4, l16);
	//_delay_ms(l8);
	tone(F4, l16);
	_delay_ms(l8);
	_delay_ms(l8);
	_delay_ms(l8);
	tone(G4, l16);
	_delay_ms(l8);
	tone(A4, l16);
	tone(B4, l16);
	_delay_ms(l8);
	tone(F5, l16);
	_delay_ms(l8);
	_delay_ms(l8);
	_delay_ms(l8);
	tone(A4, l16);
	_delay_ms(l8);
	tone(B4, l16);
	tone(C5, l16);
	_delay_ms(l8);
	_delay_ms(l8);
	tone(D5, l16);
	_delay_ms(l8);
	_delay_ms(l8);
	tone(E5, l16);
	_delay_ms(l8);
	_delay_ms(l8);
	tone(E4, l16);
	_delay_ms(l8);
	tone(F4, l16);
	tone(G4, l16);
	_delay_ms(l8);
	tone(C5, l16);
	_delay_ms(l8);
	_delay_ms(l8);
	_delay_ms(l8);
	_delay_ms(l8);
	tone(D5, l16);
	_delay_ms(l8);
	tone(E5, l16);
	tone(F5, l16);
	_delay_ms(l8);
	_delay_ms(l8);
	_delay_ms(l8);
	tone(G4, l16);
	_delay_ms(l8);
	tone(G4, l16);
	_delay_ms(l16);
	tone(E5, l16);
	_delay_ms(l8);
	_delay_ms(l8);
	tone(D5, l16);
	_delay_ms(l8);
	tone(G4, l16);
	_delay_ms(l8);
	tone(E5, l16);
	_delay_ms(l8);
	_delay_ms(l8);
	tone(D5, l16);
	_delay_ms(l8);
	tone(G4, l16);
	_delay_ms(l8);
	tone(E5, l16);
	_delay_ms(l8);
	_delay_ms(l8);
	tone(D5, l16);
	_delay_ms(l8);
	tone(G4, l16);
	_delay_ms(l8);
	tone(F5, l16);
	_delay_ms(l8);
	tone(E5, l16);
	tone(D5, l16);
	_delay_ms(l8);
	tone(C5, l16);
	tone(C5, l16);
	sei();
	// Ende des AZ: jetzt sind Interrupts wieder moeglich

	_delay_ms(l8);
	_delay_ms(l8);
	_delay_ms(l8);
	_delay_ms(l8);
	_delay_ms(l8);
	_delay_ms(l8);
	_delay_ms(l8);
	_delay_ms(l8);
	//ENDE:
}

/**
 * Main function of the NiboShorty.c file.
 * Receives a message from the other Nibo
 * if 1 than running state is 1 and Nibo plays music
 * if 0 than running state is 0 and Nibo stops music
 */
int main() {

	// initialize the robot
	bot_init();

	// initialize spi-port
	spi_init();

	// initialize the display and the graphical operations
	display_init();
	gfx_init();

	sound_init();

	// calls the function which initialize the UART0-Port
	initUART0();
	// declare and initialize a variable for storing received characters
	unsigned char message;
	int isRunning = 0;
	while (1) {
		// if the receive buffer is not empty ->
		if (!uart0_rxempty()) {
			// -> get char out of the buffer
			message = uart0_getchar();
			int byte = message;
			//if this bit is 0 than message is for the Nibo
			if ((byte & 128) != 0) {
				continue;
			}

			//if this bit is 1 than Nibo plays the music
			if ((byte & 1) == 1) {
				isRunning = 1;
			} else if ((byte & 1) == 0) {
				isRunning = 0;
			}
		}
		if (isRunning == 1) {
			gfx_move(0, 0);
			gfx_fill(0);
			//printing Indiana Jones picture on the Display
			gfx_draw_xbm_P(indigraphic_width, indigraphic_height,
					(PGM_P) indigraphic_bits);
			playIndi();
		}
		// Delay for the operation loop
		_delay_ms(10);
	}
	return 0;
}

