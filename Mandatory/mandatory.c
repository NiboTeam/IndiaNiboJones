/**
 * @file mandatory.c
 * @brief This program lets the Nibo drive free by passing obstacles automatically
 * @author Philip Mitzlaff & Benedikt Petschelt
 * @date 23.11.2018
 * @version 1.0
 */

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
// for showing the battery status
#include "batterystatus.h"
// for showing the distances to obstacles visual on the display
#include "showdistance.h"

/**if DEBUG is 0 than the Nibo shows the distances visual on the display
 else if DEBUG is 1 than the Nibo shows the different states on the display**/
#define DEBUG 0

/**
 * set the initial state to NO_OBSTACLE and define the thresholds for slopes, front and sides
 */
int currentState = 0;
int threshold_slopes = 90;
int threshold_front = 100;
int threshold_sides = 45;

/** state machine with the 6 possible states **/
enum STATES {
	NO_OBSTACLE,
	OBSTACLE_FRONT,
	OBSTACLE_LEFT,
	OBSTACLE_RIGHT,
	OBSTACLE_SIDES,
	OBSTACLE_DEADEND
};

/** function to write something on the display*/
void writeToDisplay(char *output) {
	gfx_fill(0);
	gfx_move(0, 0);
	gfx_print_text(output);
	return;
}
/**
 * lets the Nibo turn left until a specific threshold for sensor 1 and 2 is reached
 */
void turnLeft() {
	copro_setSpeed(-10, 10);
	leds_set_status(LEDS_ORANGE, 3);
	while (1 == 1) {
		copro_update();
		if (((copro_distance[2] / 256) < 50)
				&& ((copro_distance[1] / 256) < 30)) {
			copro_stop();
			leds_set_status(LEDS_OFF, 3);
			break;
		}
	}
}
/**
 * lets the Nibo turn right until a specific threshold for sensor 2 and 3 is reached
 */
void turnRight() {
	copro_setSpeed(10, -10);
	leds_set_status(LEDS_ORANGE, 6);
	while (1 == 1) {
		copro_update();
		if (((copro_distance[2] / 256) < 50)
				&& ((copro_distance[3] / 256) < 30)) {
			copro_stop();
			leds_set_status(LEDS_OFF, 6);
			break;
		}
	}
}

/**
 * The Nibo turns 90 degree in the right direction
 */
void hardRight() {
	copro_stop();
	leds_set_status(LEDS_ORANGE, 6);
	copro_setTargetRel(27, -27, 10);
	delay(2000);
	leds_set_status(LEDS_OFF, 6);
	return;
}

/**
 * The Nibo turns 90 degree in the left direction
 */
void hardLeft() {
	copro_stop();
	leds_set_status(LEDS_ORANGE, 3);
	copro_setTargetRel(-27, 27, 10);
	delay(2000);
	leds_set_status(LEDS_OFF, 3);
	return;
}

/**
 * The Nibo turns left if the distance sensor 0 is higher than the sensor 4 or
 * otherwise turn right
 */
void checkSides() {
	copro_update();
	if (copro_distance[0] / 256 > copro_distance[4] / 256) {
		turnLeft();
	} else {
		turnRight();
	}
	return;
}

/**
 * The Nibo drives backwards until the distance of one of the side sensors is
 * lower than a specific threshold and does a 90 degree turn in the opposite direction
 * or if both sensors are lower than the threshold the Nibo turns right
 */
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

/**
 * main function of the manatory.c file
 * @return
 */
int main() {
	sei();
	bot_init();
	spi_init();

	display_init();
	gfx_init();
	leds_init();

	copro_ir_startMeasure();

	/**
	 * endless loop of the main function
	 */
	while (1 == 1) {
		copro_setSpeed(10, 10);
		copro_update();

		/**
		 * query each sensor and set a specific state if several thresholds are reached
		 */
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
		/**
		 * set a specific method to avoid the obstacles dependent on which state is active
		 */
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
