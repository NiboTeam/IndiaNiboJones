/*
 * mandatoryProtocol.c
 *
 *  Created on: 06.11.2018
 *      Author: hendriktanke
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
#include "n2switchS3.h"
#include "protocols.h"
#include "gfxOutput.h"

#define OBSTACLE_IN_RANGE 20
#define FAR_DISTANCE 50
#define NEAR_DISTANCE 100
#define CRITICAL_DISTANCE 200

enum state {
	RUNNING_FORWARD,
	TURNING_LEFT,
	TURNING_RIGHT,
	WAITING,
	RUNNING_BACKWARDS,
	HARD_LEFT,
	HARD_RIGHT
} machineState;

char text[40] = "";
void writeDisplay() {

	gfx_fill(0);
	gfx_move(0, 0);
	//sprintf(text, "Speed l: %3i  r: %3i", copro_speed_l, copro_speed_r);
	gfx_print_text(text);
}
void driveBackwards() {
	while (1 == 1) {
		copro_setSpeed(-10, -10);
		copro_update();
		if (copro_distance[0] / 256 < 20 && copro_distance[4] / 256 < 20) {
			delay(1000);
			turnRight();
			break;
		} else if (copro_distance[0] / 256 < 25) {
			delay(1000);
			turnRight();
			break;;
		} else {
			delay(1000);
			turnLeft();
			break;
		}
		delay(1000);
	}
}
void hardRight() {
	copro_stop();
	copro_setTargetRel(27, -27, 10);
	delay(2000);
}
void hardLeft() {
	copro_stop();
	copro_setTargetRel(-27, 27, 10);
	delay(2000);
}
void turnLeft() {
	copro_setSpeed(-10, 10);
	while (1 == 1) {
		copro_update();
		if (copro_distance[2] / 256 < 30) {
			copro_setSpeed(10, 10);
			return;
		}
		delay(500);
	}
}
void turnRight() {
	copro_setSpeed(10, -10);
	while (1 == 1) {
		copro_update();
		if (copro_distance[2] / 256 < 30) {
			copro_setSpeed(10, 10);
			return;
		}
		delay(500);
	}
}
/*
 * This will check whether a sensor is running over threshold.
 * @param sensorID id of sensor
 * @param thresholdDistanceValue threshold value for distance sensor
 * @return 0 if threshold is not reached. Otherwise 1.
 */
int obstacleInSight(int sensorID, int thresholdDistanceValue) {
	int current_distance = copro_distance[sensorID] / 256;
	if (current_distance >= thresholdDistanceValue) {
		return 1;
	}
	return 0;
}

/*
 * This will compare two value for the smallest. The smallest value will be indicated by 1 or 0.
 * @param value1
 * @param value2
 * @return 0 if value1 is smaller then value2. Otherwise 1.
 */
int compareForSmallestValue(int value1, int value2) {
	if (value1 < value2) {
		return 0;
	} else {
		return 1;
	}
}

int main() {
	machineState = WAITING;
	int last_machineState = RUNNING_BACKWARDS;
	int loopCounter = 0;
	printDebug("Protocoll started!");

	sei();
	bot_init();
	spi_init();

	display_init();
	gfx_init();

	copro_ir_startMeasure();
	// Endlosschleife
	while (1 == 1) {
		copro_update();

		if (s3_was_pressed()) {
			if (machineState != WAITING) {
				machineState = WAITING;
			} else if (machineState == WAITING) {
				machineState = RUNNING_FORWARD;
			}
		}

		//preload all Values (vielleicht eine Sensorabfrage pro Durchlauf,
		//um die Durchlauf Geschwindigkeit zu erhöhen?)
		int front = obstacleInSight(2, CRITICAL_DISTANCE);
		int lBlade = obstacleInSight(1, NEAR_DISTANCE);
		int rBlade = obstacleInSight(3, NEAR_DISTANCE);
		int lSide = obstacleInSight(0, NEAR_DISTANCE);
		int rSide = obstacleInSight(4, NEAR_DISTANCE);
		switch (machineState) {
		case RUNNING_FORWARD:
			if (front) { //Hindernis gefunden
				if (front && lBlade && rBlade) { //Ist das Hindernis zu groß?
					if (rSide && lSide) { // Ist es vielleicht sogar eine Sackgasse?
						machineState = RUNNING_BACKWARDS;
					} else {
						if (compareForSmallestValue(copro_distance[1] / 256,
								copro_distance[3] / 256) == 1) {
							machineState = TURNING_LEFT;
						} else {
							machineState = TURNING_RIGHT;
						}
					}
				}
			} else if (rBlade || lBlade) {
				if (compareForSmallestValue(copro_distance[1] / 256,
						copro_distance[3] / 256) == 1) {
					machineState = TURNING_LEFT;
				} else {
					machineState = TURNING_RIGHT;
				}
			} else {
				copro_setSpeed(10, 10);
			}
			break;
		case TURNING_LEFT:
			turnLeft();
			break;
		case TURNING_RIGHT:
			turnRight();
			break;
		case RUNNING_BACKWARDS:
			driveBackwards();
			break;
		case WAITING:
			copro_stop();
			break;
		case HARD_LEFT:
			hardLeft();
			break;
		case HARD_RIGHT:
			hardRight();
			break;
		}

		if (last_machineState != machineState) {
			printMachineState((int) machineState);
		}

		last_machineState = machineState;
		loopCounter++;

		//gfx_fill(0);
		//copro_stop();
		//delay(800);

		//copro_stopImmediate();
		//copro_resetOdometry(0, 0);
		//copro_setTargetAbs(280, 280, 28);
		//copro_setTargetRel(-420, -420, 28);
		//copro_resetOdometry(0, 0);
	}
}

