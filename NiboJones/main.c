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
#include <nibo/leds.h>
#include <nibo/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>

#include "n2switchS3.h"
#include "protocols.h"
#include "gfxOutput.h"
#include "uart0.h"

int coproTicksLeft;
int coproTicksRight;
int send_counter = 0;

void initUART0(){
	uart0_set_baudrate(9600);
	uart0_enable();
}

void gatherCoproData(){
	coproTicksLeft = copro_ticks_l;
	coproTicksRight = copro_ticks_r;
}

int sendSegment(int recognizedLine, int direction, int leftOdometry, int rightOdometry){
	int averageOdometry = ((leftOdometry + rightOdometry) / 27) / 2;
	// 1 * 2 ^ 7 addresses the PC
	int byte = 128 + (64 * recognizedLine) + (32 * direction) + averageOdometry;
	unsigned char message = byte & 255;
	unsigned char answer;
	int finishedSuccessfull = 0;
	int i = 0;
	while(1){
		if(!uart0_txfull()){
			uart0_putchar(message);
		}
		delay(1000);
		// wait for answer
		if(!uart0_rxempty()){
			answer = uart0_getchar();
			delay(500);
			int byte = answer;
			if((byte & 128) != 128){
				// wrong indicator
				continue;
			}
			finishedSuccessfull = 1;
			break;
		}else if (i == 5){
			finishedSuccessfull = 0;
			break;
		}
		i++;
	}
	return finishedSuccessfull;
}

void sendMessageToNibo(int signal){
	// signal indicates the start or end of the music
	// this message addresses the NIBO, so the indication bit has to be 0
	int byte = signal;
	unsigned char message = byte & 255;
	unsigned char answer;
	int i = 0;
	while(1){
		if(!uart0_txfull()){
			uart0_putchar(message);
		}
		delay(200);
		if (!uart0_rxempty()){
			// wait for answer
			answer = uart0_getchar();
			byte = answer;
			if((byte & 128) != 0){
				// wrong indicator
				continue;
			}
			break;
		}else if (i == 25){
			break;
		}
		i++;
	}
}

int sendData(){
	int successful = 0;
	gatherCoproData();
	cleanDebug(20);
	printDebug(send_counter + " start sending.");
	successful = sendSegment(getReconizedBlackLine(), getTurnDirection(), coproTicksLeft, coproTicksRight);
	cleanDebug(20);
	printDebug(send_counter + " finished sending.");
	send_counter++;
	copro_resetOdometry(0, 0);

	return successful;
}

int main() {
	machine_State = WAITING;
	measure_State = FINISHED;
	run_direction = UNKNOWN;
	int isInitialized = 0;
	int last_machineState = FINISHING_RUN;
	printDebug("Protocol started!");

	sei();
	bot_init();
	spi_init();
	leds_init();

	initUART0();

	display_init();
	gfx_init();

	copro_ir_startMeasure();
	floor_init();
	int sendingResult;
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
			leds_set_status(LEDS_OFF, 3);
			leds_set_status(LEDS_OFF, 7);
			leds_set_status(LEDS_RED, 0);
			leds_set_status(LEDS_RED, 1);
			turnInside_protocol();
			leds_set_status(LEDS_OFF, 0);
			leds_set_status(LEDS_OFF, 1);
			if(measure_State == STARTED){
				machine_State = SENDING_DATA;
			}else{
				machine_State = RUNNING_FORWARD;
			}
			break;
		case TURNING_OUTSIDE:
			leds_set_status(LEDS_OFF, 3);
			leds_set_status(LEDS_OFF, 7);
			leds_set_status(LEDS_RED, 0);
			leds_set_status(LEDS_RED, 1);
			turnOutside_protocol();
			machine_State = RUNNING_FORWARD;
			leds_set_status(LEDS_OFF, 0);
			leds_set_status(LEDS_OFF, 1);
			copro_setSpeed(10, 10);
			delay(2000);
			if(measure_State == STARTED){
				machine_State = SENDING_DATA;
			}else{
				machine_State = RUNNING_FORWARD;
			}
			break;
		case SENDING_DATA:
			sendingResult = sendData();
			if(sendingResult == 1){
				if(measure_State == FINISHED){
					machine_State = FINISHING_RUN; //FINISHING_RUN
				}else if(sendingResult == 1 && measure_State == STARTED){
					machine_State = RUNNING_FORWARD;
				}
			} else {
				machine_State = FINISHING_RUN;
				measure_State = FINISHED;
				printDebug("Something went wrong!");
			}
			setReconizedBlackLine(0);
			break;
		case FINISHING_RUN:
			copro_stop();
			finishRun_protocol(); //Umriss anzeigen
			machine_State = WAITING;
			break;
		case WAITING:
			copro_stop();
			if (s3_was_pressed()) {
				if(isInitialized == 1){
					machine_State = RUNNING_FORWARD;
				}else{
					machine_State = INITIALIZATION;
					isInitialized = 1;
				}

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

