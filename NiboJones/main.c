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

int measuredData = 0;

void processGatheredData(){
	if(measure_State == STARTED){
		measuredData = (copro_ticks_l + copro_ticks_r) / 2; //calculate average value
	}
}

int sendData(){
	int succesfull = 0;
	if(measure_State == STARTED){
		//send measuredData!
		copro_resetOdometry(0, 0);
	}

	return succesfull;
}

int main() {
	machine_State = WAITING;
	measure_State = FINISHED;
	run_direction = UNKNOWN;
	int last_machineState = FINISHING_RUN;
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
		switch (machine_State) {
		case INITIALIZATION:
			init_protocol();
			machine_State = RUNNING_FORWARD;
			break;
		case RUNNING_FORWARD:
			runForward_protocol();
			break;
		case TRACK_CORRECTION:
			trackCorrection_protocol();
			break;
		case TURNING_INSIDE:
			processGatheredData();
			turnInside_protocol();
			sendData();
			if(measure_State == STARTED){
				machine_State = SENDING_DATA;
			}else{
				machine_State = RUNNING_FORWARD;
			}
			break;
		case TURNING_OUTSIDE:
			processGatheredData();
			turnOutside_protocol();
			sendData();
			machine_State = RUNNING_FORWARD;
			copro_setSpeed(10, 10);
			delay(2000);
			if(measure_State == STARTED){
							machine_State = SENDING_DATA;
						}else{
							machine_State = RUNNING_FORWARD;
						}
			break;
		case SENDING_DATA:
			if(sendData() && measure_State == FINISHED){
				machine_State = FINISHING_RUN;
			}else if(sendData() && measure_State == STARTED){
				machine_State = RUNNING_FORWARD;
			}
			//sonst im machine_state beleiben
			break;
		case FINISHING_RUN:
			copro_stop();
			finishRun_protocol(); //Umriss anzeigen
			machine_State = WAITING;
			break;
		case WAITING:
			copro_stop();
			if (s3_was_pressed()) {
				machine_State = RUNNING_FORWARD;
			}
			break;
		}

		if (last_machineState != machine_State) {
			printMachineState(machine_State);
		}

		last_machineState = machine_State;

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

