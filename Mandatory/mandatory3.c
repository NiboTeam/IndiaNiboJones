// Standard Includes
#include <nibo/niboconfig.h>
#include <nibo/iodefs.h>
#include <nibo/bot.h>
// Display
#include <nibo/display.h>
// Grafikfunktionen
#include <nibo/gfx.h>
// Wartefunktionen
#include <nibo/delay.h>
// Coprozessorfunktionen
#include <nibo/copro.h>
// Kommunikationsfunktionen
#include <nibo/spi.h>
// Interrupts
#include <avr/interrupt.h>
// Ein- und Ausgabefunktionen
#include <stdio.h>

#include <nibo/leds.h>

#include "batterystatus.h"

#include "showdistance.h"

//if DEBUG is 0 than the Nibo shows the distances visual on the display
// else if DEBUG is 1 than the Nibo shows the different states on the display
#define DEBUG 0

int currentState = 0;
int threshold_slopes = 90;
int threshold_front = 100;
int threshold_sides = 45;

enum STATES {
	NO_OBSTACLE,
	OBSTACLE_FRONT,
	OBSTACLE_LEFT,
	OBSTACLE_RIGHT,
	OBSTACLE_SIDES,
	OBSTACLE_DEADEND
};
void writeToDisplay(char *output) {
	gfx_fill(0);
	gfx_move(0, 0);
	gfx_print_text(output);
	return;
}
void turnLeft() {
	copro_setSpeed(-10, 10);
	leds_set_status(LEDS_ORANGE, 3);
	while (1 == 1) {
		copro_update();
		if (((copro_distance[2] / 256) < 50) && ((copro_distance[1] / 256) < 30)) {
			//delay(500);
			copro_stop();
			leds_set_status(LEDS_OFF, 3);
			break;
		}
	}
}
void turnRight() {
	copro_setSpeed(10, -10);
	leds_set_status(LEDS_ORANGE, 6);
	while (1 == 1) {
		copro_update();
		if (((copro_distance[2] / 256) < 50) && ((copro_distance[3] / 256) < 30)) {
			//delay(500);
			copro_stop();
			leds_set_status(LEDS_OFF, 6);
			break;
		}
	}
}

void hardRight() {
	copro_stop();
	leds_set_status(LEDS_ORANGE, 6);
	copro_setTargetRel(27, -27, 10);
	delay(2000);
	leds_set_status(LEDS_OFF, 6);
	return;
}
void hardLeft() {
	copro_stop();
	leds_set_status(LEDS_ORANGE, 3);
	copro_setTargetRel(-27, 27, 10);
	delay(2000);
	leds_set_status(LEDS_OFF, 3);
	return;
}

void checkSides() {
	copro_update();
	if (copro_distance[0] / 256 > copro_distance[4] / 256) {
		turnLeft();
	} else {
		turnRight();
	}
	return;
}
void driveBackwards() {
	leds_set_status(LEDS_ORANGE, 0);
	leds_set_status(LEDS_ORANGE, 1);
	copro_setSpeed(-10, -10);
	while (1 == 1) {
		copro_update();
		if (copro_distance[0] / 256 < 20 && copro_distance[4] / 256 < 20) {
			delay(500);
			copro_stop();
			leds_set_status(LEDS_OFF, 0);
			leds_set_status(LEDS_OFF, 1);
			hardRight();
			return;
		} else if (copro_distance[0] / 256 < 20) {
			delay(500);
			copro_stop();
			leds_set_status(LEDS_OFF, 0);
			leds_set_status(LEDS_OFF, 1);
			hardRight();
			return;
		} else if (copro_distance[4] / 256 < 20) {
			delay(500);
			copro_stop();
			leds_set_status(LEDS_OFF, 0);
			leds_set_status(LEDS_OFF, 1);
			hardLeft();
			return;
		}
		delay(500);
	}
}

int main() {
	sei();
	bot_init();
	spi_init();

	display_init();
	gfx_init();
	leds_init();

	copro_ir_startMeasure();
	// Endlosschleife
	while (1 == 1) {
		copro_setSpeed(10, 10);
		copro_update();
		if (copro_distance[2] / 256 >= threshold_front) {
			if ((copro_distance[0] / 256 >= threshold_sides
					&& copro_distance[4] / 256 >= threshold_sides)
					|| (copro_distance[1] / 256 >= threshold_slopes
							&& copro_distance[3] / 256 >= threshold_slopes)) {
				currentState = OBSTACLE_DEADEND;
			} else {
				currentState = OBSTACLE_FRONT;
			}
		} else if (copro_distance[1] / 256 >= threshold_slopes
				&& copro_distance[3] / 256 < threshold_slopes) {
			currentState = OBSTACLE_RIGHT;
		} else if (copro_distance[3] / 256 >= threshold_slopes
				&& copro_distance[1] / 256 < threshold_slopes) {
			currentState = OBSTACLE_LEFT;
		} else if (copro_distance[4] / 256 >= threshold_sides
				&& copro_distance[0] / 256 >= threshold_sides) {
			currentState = OBSTACLE_SIDES;
		} else {
			currentState = NO_OBSTACLE;
		}

		if (DEBUG == 0) {
			gfx_fill(0);
			batteryStatus();
			showDistance(copro_distance[0] / 256, copro_distance[1] / 256,
					copro_distance[2] / 256, copro_distance[3] / 256,
					copro_distance[4] / 256);
		}
		switch (currentState) {
		case NO_OBSTACLE:
			if (DEBUG == 1) {
				writeToDisplay("NO OBSTACLE");
			}
			break;
		case OBSTACLE_FRONT:
			copro_stop();
			if (DEBUG == 1) {
				writeToDisplay("OBSTACLE: FRONT");
			}
			checkSides();
			break;
		case OBSTACLE_LEFT:
			if (DEBUG == 1) {
				writeToDisplay("OBSTACLE: LEFT");
			}
			turnRight();
			break;
		case OBSTACLE_RIGHT:
			if (DEBUG == 1) {
				writeToDisplay("OBSTACLE: RIGHT");
			}
			turnLeft();
			break;
		case OBSTACLE_SIDES:
			if (DEBUG == 1) {
				writeToDisplay("OBSTACLE: TUNNEL");
			}
			break;
		case OBSTACLE_DEADEND:
			copro_stop();
			if (DEBUG == 1) {
				writeToDisplay("OBSTACLE: DEADEND");
			}
			driveBackwards();
			break;
		}
		delay(500);
	}
}
