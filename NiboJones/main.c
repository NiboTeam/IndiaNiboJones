/*
 * mandatoryProtocol.c
 *
 *  Created on: 06.11.2018
 *      Author: hendriktanke
 */

#include <nibo/niboconfig.h>
#include <nibo/iodefs.h>
#include <nibo/bot.h>
#include <nibo/display.h>
#include <nibo/floor.h>
#include <nibo/gfx.h>
#include <nibo/delay.h>
#include <nibo/copro.h>
#include <nibo/spi.h>
#include <nibo/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>

#include "n2switchS3.h"
#include "protocols.h"
#include "gfxOutput.h"

#define BLACK_UNDERGROUND 10

int measuredData = 0;

enum measurement {
	FINISHED = 0, STARTED = 1
} measure_State;

void button_action() {
	if (s3_was_pressed()) {
		if (machine_state != WAITING) {
			machine_state = WAITING;
		} else if (machine_state == WAITING) {
			machine_state = RUNNING_FORWARD;
			init_protocol();
		}
	}
}

void processGatheredData(){
	if(measure_State == STARTED){
		measuredData = (copro_ticks_l + copro_ticks_r) / 2; //calculate average value
	}
}

void sendData(){
	if(measure_State == STARTED){
		//send measuredData!
		copro_resetOdometry(0, 0);
	}
}

/**
 * use this function if you like to know, whether the color of the underground hits the defined threshold.
 * @return 1 if underground color is bigger than defined border.
 */
int checkForBlackUnderground(){
	int blackUndergroundDetected = 0;
	if(floor_relative[FLOOR_RIGHT] > BLACK_UNDERGROUND || floor_relative[FLOOR_LEFT] > BLACK_UNDERGROUND){
		blackUndergroundDetected = 1;
	}

	return blackUndergroundDetected;
}

int main() {
	machine_state = WAITING;
	run_direction = UNKNOWN;
	int last_machineState = END;
	int last_measureState = FINISHED;
	printDebug("Protocol started!");

	sei();
	bot_init();
	spi_init();

	display_init();
	gfx_init();

	copro_ir_startMeasure();
	floor_init();

	// Endlosschleife
	while (1 == 1) {
		button_action(); //stopped or not stopped by user manually.

		switch (machine_state) {
		case RUNNING_FORWARD:
			runForward_protocol();
			break;
		case TURNING_LEFT:
			processGatheredData();
			leftTurn_protocol();
			sendData();
			machine_state = RUNNING_FORWARD;
			break;
		case TURNING_RIGHT:
			processGatheredData();
			rightTurn_protocol();
			sendData();
			machine_state = RUNNING_FORWARD;
			break;
		case END:
			break;
		case WAITING:
			copro_stop();
			break;
		}
		floor_update();
		if (last_measureState != measure_State) {
			switch (measure_State) {
			case FINISHED:
				cleanDebug("        ");
				printDebug("Finished!");
				if(checkForBlackUnderground()){
					measure_State = STARTED;
				}
				break;
			case STARTED:
				copro_resetOdometry(0, 0);
				cleanDebug("         ");
				printDebug("Started!");
				if(checkForBlackUnderground()){
					measure_State = FINISHED;
				}
				break;
			}
		}

		if (last_machineState != machine_state) {
			printMachineState(machine_state);
		}

		last_machineState = machine_state;

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

