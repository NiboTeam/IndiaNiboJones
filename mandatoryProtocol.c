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
} mashineState;

char text[40] = "";
void writeDisplay() {
	gfx_fill(0);
	gfx_move(0, 0);
	//sprintf(text, "Speed l: %3i  r: %3i", copro_speed_l, copro_speed_r);
	gfx_print_text(text);
}

int obstacleInSight(int sensorID, int thresholdDistanceValue);

int obstacleInSight(int sensorID, int thresholdDistanceValue) {
	int current_distance = copro_distance[sensorID] / 256;
	if (current_distance >= thresholdDistanceValue) {
		return 1;
	}
	return 0;
}

int decideLeftOrRight(int leftSensorValue, int rightSensorValue) {
	if (leftSensorValue < rightSensorValue) {
		return 1;
	} else {
		return 0;
	}
}

int main() {
	mashineState = WAITING;
	sei();
	bot_init();
	spi_init();

	display_init();
	gfx_init();

	copro_ir_startMeasure();
	int current_distance = 0;
	// Endlosschleife
	while (1 == 1) {
		copro_update();
		copro_setSpeed(10, 10);
		int front = obstacleInSight(2, CRITICAL_DISTANCE);
		int lBlade = obstacleInSight(1, NEAR_DISTANCE);
		int rBlade = obstacleInSight(3, NEAR_DISTANCE);
		int lSide = obstacleInSight(0, NEAR_DISTANCE);
		int rSide = obstacleInSight(4, NEAR_DISTANCE);
		int current_distance;
		switch (mashineState) {
		case RUNNING_FORWARD:
			if (front) { //Hindernis gefunden
				if (front && lBlade && rBlade) { //Ist das Hindernis zu groß?
					if (rSide && lSide) { // Ist es vielleicht sogar eine Sackgasse?
						mashineState = RUNNING_BACKWARDS;
					}
				}
				if(decideLeftOrRight(copro_distance[1] / 256,
						copro_distance[3] / 256)==1){
					mashineState == TURNING_LEFT;
				}else{
					mashineState == TURNING_RIGHT;
				}
			}
			break;
		case TURNING_LEFT:
			//nach links abdrehen - abhängig von Distanzsensoren
			break;
		case TURNING_RIGHT:
			//Nach rechts abdrehen - abhängig von Distanzsensoren
			break;
		case RUNNING_BACKWARDS:
			//Rückwärts fahren und Wände beobachten
			break;
		case WAITING:
			//Warten bis Knöpfchen gedrückt dann RUNNING_FORWARD
			break;
		case HARD_LEFT:
			//Bremsen und 90° nach links
			break;
		case HARD_RIGHT:
			//Bremsen und 90° nach rechts
			break;
		}

		copro_stop();
		// 2s warten
		delay(500);

		copro_stopImmediate();
		copro_resetOdometry(0, 0);
		copro_setTargetAbs(280, 280, 28);
		copro_setTargetRel(-420, -420, 28);
		copro_resetOdometry(0, 0);
	}
}

