/*
 ============================================================================
 Name        : protocols.c
 Author      :
 Version     :
 Description : Holds different protocols and util-methods for controlling nibo
 ============================================================================
 */
#include <nibo/niboconfig.h>
#include <nibo/iodefs.h>
#include <nibo/bot.h>

#include <nibo/copro.h>
#include <nibo/leds.h>
#include <nibo/spi.h>
#include <nibo/delay.h>
#include "gfxOutput.h"
#include <nibo/floor.h>
#include "protocols.h"
#include <nibo/delay.h>
#include "uart0.h"
#include "n2switchS3.h"

//#define FRONT_THRESHOLD 200
#define SIDE_OUTSIDE_TURN_THRESHOLD 20
/*#define SIDE_THRESHOLD_OUTSIDE_MAX 200 //180
#define SIDE_THRESHOLD_OUTSIDE_MIN 190 //150
#define BLADE_THRESHOLD_OUTSIDE_MAX 100
#define BLADE_THRESHOLD_OUTSIDE_MIN 50*/
#define BLACK_UNDERGROUND 40

enum correctState {
	OUTSIDE = 0,
	INSIDE = 1,
	NON
} correctState;

int FRONT_THRESHOLD = 0;
int SIDE_THRESHOLD_OUTSIDE_MAX = 0; //180
int SIDE_THRESHOLD_OUTSIDE_MIN = 0; //150
int BLADE_THRESHOLD_OUTSIDE_MAX = 0;
int BLADE_THRESHOLD_OUTSIDE_MIN = 0;
int lastCorrectState = -1;
int reconizedBlackLine_send = 0;
int turnDirection_send = 0;
int sensorIDSide;
int sensorIDBlade;

int counter = 0;

void setReconizedBlackLine(int reconized){
	reconizedBlackLine_send = reconized;
}
int getReconizedBlackLine(){
	return reconizedBlackLine_send;
}

int getTurnDirection(){
	return turnDirection_send;
}

void sendMessageToNibo(int signal){
	// signal indicates the start or end of the music
	// this message addresses the NIBO, so the indication bit has to be 0
	int byte = signal;
	unsigned char message = byte & 255;
	unsigned char answer;
	int i = 0;
	while(i == 0){
		if(!uart0_txfull()){
			uart0_putchar(message);
		}
		/*delay(200);
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
		}*/
		i++;
	}
}

void init_protocol(){
	floor_init();
	copro_update();
	printDebug("Initialization...");
	printMovingDirection(run_direction);
	int averageDistance_SideLeft = 0;
	int averageDistance_SideRight = 0;
	int averageDistance_BladeLeft = 0;
	int averageDistance_BladeRight= 0;

	for(int i = 0; i < 5; i++){
		copro_update();
		averageDistance_SideLeft += copro_distance[4] / 256;
		averageDistance_SideRight += copro_distance[0] / 256;
		averageDistance_BladeLeft += copro_distance[3] / 256;
		averageDistance_BladeRight += copro_distance[1] / 256;
		delay(200);
	}

	averageDistance_SideLeft = averageDistance_SideLeft / 5;
	averageDistance_SideRight = averageDistance_SideRight / 5;
	averageDistance_BladeLeft = averageDistance_BladeLeft / 5;
	averageDistance_BladeRight = averageDistance_BladeRight / 5;

	if(averageDistance_SideLeft > averageDistance_SideRight){
		run_direction = RIGHT_RUN;
		SIDE_THRESHOLD_OUTSIDE_MAX = averageDistance_SideLeft;
		SIDE_THRESHOLD_OUTSIDE_MIN = averageDistance_SideLeft - 20;
		BLADE_THRESHOLD_OUTSIDE_MAX = averageDistance_BladeLeft;
		BLADE_THRESHOLD_OUTSIDE_MIN = averageDistance_BladeLeft - 20;
	}else{
		run_direction = LEFT_RUN;
		SIDE_THRESHOLD_OUTSIDE_MAX = averageDistance_SideRight + 10;
		SIDE_THRESHOLD_OUTSIDE_MIN = averageDistance_SideRight - 10;
		BLADE_THRESHOLD_OUTSIDE_MAX = averageDistance_BladeRight + 10;
		BLADE_THRESHOLD_OUTSIDE_MIN = averageDistance_BladeRight - 10;
	}

	FRONT_THRESHOLD = SIDE_THRESHOLD_OUTSIDE_MAX;

	if(SIDE_THRESHOLD_OUTSIDE_MAX > 250){
		FRONT_THRESHOLD = FRONT_THRESHOLD - 30;
	}else if(SIDE_THRESHOLD_OUTSIDE_MAX > 230){
		FRONT_THRESHOLD = FRONT_THRESHOLD - 10;
	}

	if(run_direction == LEFT_RUN){
			sensorIDSide = 0;
			sensorIDBlade = 1;
	}else{
			sensorIDSide = 4;
			sensorIDBlade = 3;
	}

	cleanDebug(17);
	/*if(run_direction == LEFT_RUN){
		printDebug("L");
	}else{
		printDebug("R");
	}*/
	delay(500);
	printMovingDirection(run_direction);
}

void correctTrackToLeft(){
	leds_set_status(LEDS_ORANGE, 4);
	leds_set_status(LEDS_OFF, 5);
	copro_setSpeed(10, 12);
}

void correctTrackToRight(){
	leds_set_status(LEDS_ORANGE, 5);
	leds_set_status(LEDS_OFF, 4);
	copro_setSpeed(12, 10);
}

void correctTrackToInside(int direction){
	if(direction == LEFT_RUN){
		correctTrackToLeft();
	}else if(direction == RIGHT_RUN){
		correctTrackToRight();
	}
}

void correctTrackToOutside(int direction){
	if(direction == LEFT_RUN){
		correctTrackToRight();
	}else if(direction == RIGHT_RUN){
		correctTrackToLeft();
	}
}

void leftTurnFree() {
	turnDirection_send = 0;
	leds_set_status(LEDS_ORANGE, 3);
	while (1 == 1) {
		copro_update();
		copro_setSpeed(-10, 15);
		if (copro_distance[2] / 256 < 100 && copro_distance[1] / 256 < 50) {
			copro_stop();
			leds_set_status(LEDS_OFF, 3);
			return;
		}
	}
}

void leftTurnForced() {
	turnDirection_send = 0;
	leds_set_status(LEDS_ORANGE, 3);
	copro_setTargetRel(-27, 27, 10);
	delay(2000);
	leds_set_status(LEDS_OFF, 3);
}

void rightTurnFree() {
	turnDirection_send = 1;
	leds_set_status(LEDS_ORANGE, 6);
	while (1 == 1) {
		copro_update();

		copro_setSpeed(15, -10);
		if (copro_distance[2] / 256 < 100 && copro_distance[3] / 256 < 50) {
			copro_stop();
			leds_set_status(LEDS_OFF, 6);
			return;
		}
	}
}

void rightTurnForced() {
	turnDirection_send = 1;
	leds_set_status(LEDS_ORANGE, 6);
	copro_setTargetRel(27, -27, 10);
	delay(2000);
	leds_set_status(LEDS_OFF, 6);
}

void turnInside_protocol(){
	if(run_direction == LEFT_RUN){
		leftTurnFree();
	}else{
		rightTurnFree();
	}
}

void turnOutside_protocol(){
	if(run_direction == LEFT_RUN){
		rightTurnForced();
	}else{
		leftTurnForced();
	}
	delay(1000);
}

void trackCorrection_protocol(){
	copro_update();
	if(copro_distance[sensorIDSide] / 256 > SIDE_THRESHOLD_OUTSIDE_MAX || copro_distance[sensorIDBlade] / 256 > BLADE_THRESHOLD_OUTSIDE_MAX){
		correctState = INSIDE;
		correctTrackToInside(run_direction);
	}else if(copro_distance[sensorIDSide] / 256 < SIDE_THRESHOLD_OUTSIDE_MIN || copro_distance[sensorIDBlade] / 256 < BLADE_THRESHOLD_OUTSIDE_MIN) {
		correctState = OUTSIDE;
		correctTrackToOutside(run_direction);
	}else{
		correctState = NON;
		leds_set_status(LEDS_OFF, 4);
		leds_set_status(LEDS_OFF, 5);
		copro_setSpeed(10, 10);
	}

	if(lastCorrectState != correctState){
		//cleanDebug(15);
		if(correctState == INSIDE){
			//printDebug("Correct INSIDE!");
		} else if(correctState == OUTSIDE){
			//printDebug("Correct OUTSIDE!");
		} else{
			//printDebug("No correction!");
		}
	}
	lastCorrectState = correctState;
	machine_State = RUNNING_FORWARD;
}

int floorCheck(){
	floor_update();

	if(reconizedBlackLine_send == 0 && (floor_relative[FLOOR_LEFT] <= BLACK_UNDERGROUND || floor_relative[FLOOR_RIGHT] <= BLACK_UNDERGROUND)){
		switch (measure_State) {
			case FINISHED:
				cleanDebug("        ");
				printDebug("Started!");
				copro_resetOdometry(0, 0);
				measure_State = STARTED;
				copro_stop();
				sendMessageToNibo(1);
				//sendMessageToNibo(1);
				break;
			case STARTED:
				cleanDebug("         ");
				printDebug("Finished!");
				measure_State = FINISHED;
				machine_State = SENDING_DATA;
				copro_stop();
				sendMessageToNibo(0);
				//sendMessageToNibo(0);
				break;
		}
		reconizedBlackLine_send = 1;
		return 1;
	}
	return 0;
}

void finishRun_protocol(){
	//hier Grundrissanzeige und Empfangskommunikation einfügen.
	return;
}

void runForward_protocol(){
	copro_update();
	counter++;
	int front = copro_distance[2] / 256;
	int side_outside = copro_distance[run_direction] / 256;

	if (s3_was_pressed()) {
		machine_State = WAITING;
		return;
	}

	/*if(counter % 800 == 0 || (counter % 800) - 1 == 0 || (counter % 800) + 1 == 0){
		printInfo("                        ", 40);
		printInfo("                        ", 47);
		char output[20];
		sprintf(output, "Front: %3i ", front);
		printInfo(output, 40);
		sprintf(output, "Side: %3i ", side_outside);
		printInfo(output, 47);
	}*/

	if(floorCheck() == 1){
		return;
	}

	if (front >= FRONT_THRESHOLD) { //vorne Wand gefunden -> Innenkurve
		copro_stop();
		machine_State = TURNING_INSIDE;
	} else if(run_direction != UNKNOWN && side_outside < SIDE_OUTSIDE_TURN_THRESHOLD){ // Seitenwand weg -> Außenkurve
		machine_State = TURNING_OUTSIDE;
		delay(1000);
		copro_stop();
	} else { //freie Bahn
		machine_State = TRACK_CORRECTION;
	}
}
