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
#include "gfxOutput.h"
#include <nibo/floor.h>
#include "protocols.h"
#include <nibo/delay.h>
#include "nibotopc.h"
#include "nibotonibo.h"
#include "n2switchS3.h"

#define FRONT_THRESHOLD 100
#define SIDE_OUTSIDE_TURN_THRESHOLD 20
#define SIDE_THRESHOLD_OUTSIDE_MAX 200 //180
#define SIDE_THRESHOLD_OUTSIDE_MIN 190 //150
#define BLADE_THRESHOLD_OUTSIDE_MAX 80
#define BLADE_THRESHOLD_OUTSIDE_MIN 20
#define BLACK_UNDERGROUND 25

enum correctState {
	OUTSIDE = 0,
	INSIDE = 1,
	NON
} correctState;

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

void init_protocol(){
	floor_init();
	copro_update();
	printDebug("Initialization...");
	printMovingDirection(run_direction);
	int averageDistance_left = 0;
	int averageDistance_right = 0;

	for(int i = 0; i < 5; i++){
		averageDistance_left += copro_distance[4] / 256;
		averageDistance_right += copro_distance[0] / 256;
		delay(200);
	}

	averageDistance_left = averageDistance_left / 5;
	averageDistance_right = averageDistance_right / 5;

	if(averageDistance_left > averageDistance_right){
		run_direction = RIGHT_RUN;
	}else{
		run_direction = LEFT_RUN;
	}
	if(run_direction == LEFT_RUN){
			sensorIDSide = 0;
			sensorIDBlade = 1;
	}else{
			sensorIDSide = 3;
			sensorIDBlade = 4;
	}

	cleanDebug(17);
	if(run_direction == LEFT_RUN){
		printDebug("L");
	}
	else{
		printDebug("R");
	}
	delay(500);
	printMovingDirection(run_direction);
}

void correctTrackToLeft(){
	leds_set_status(LEDS_ORANGE, 3);
	leds_set_status(LEDS_OFF, 6);
	copro_setSpeed(10, 12);
	//delay(200);
	/*copro_setSpeed(12, 10);
	delay(200);
	copro_setSpeed(10, 10);*/
}

void correctTrackToRight(){
	leds_set_status(LEDS_ORANGE, 6);
	leds_set_status(LEDS_OFF, 3);
	copro_setSpeed(12, 10);
	//delay(200);
	/*copro_setSpeed(10, 12);
	delay(200);
	copro_setSpeed(10, 10);*/
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
	leds_set_status(LEDS_ORANGE, 2);
	while (1 == 1) {
		copro_update();
		copro_setSpeed(-10, 15);
		if (copro_distance[2] / 256 < FRONT_THRESHOLD && copro_distance[1] / 256 < 50) {
			copro_stop();
			leds_set_status(LEDS_OFF, 2);
			return;
		}
	}
}

void leftTurnForced() {
	turnDirection_send = 0;
	leds_set_status(LEDS_ORANGE, 2);
	copro_setTargetRel(-27, 27, 10);
	delay(2000);
	leds_set_status(LEDS_OFF, 2);
}

void rightTurnFree() {
	turnDirection_send = 1;
	leds_set_status(LEDS_ORANGE, 7);
	while (1 == 1) {
		copro_update();

		copro_setSpeed(15, -10);
		if (copro_distance[2] / 256 < FRONT_THRESHOLD && copro_distance[3] / 256 < 50) {
			copro_stop();
			leds_set_status(LEDS_OFF, 7);
			return;
		}
	}
}

void rightTurnForced() {
	turnDirection_send = 1;
	leds_set_status(LEDS_ORANGE, 7);
	copro_setTargetRel(27, -27, 10);
	delay(2000);
	leds_set_status(LEDS_OFF, 7);
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

void stopCorrection(int lastCorrectState){
	if(lastCorrectState == INSIDE){
		correctTrackToOutside(run_direction);
	}else if(lastCorrectState == OUTSIDE){
		correctTrackToInside(run_direction);
	}
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
		stopCorrection(lastCorrectState);
		correctState = NON;
		leds_set_status(LEDS_OFF, 3);
		leds_set_status(LEDS_OFF, 6);
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
				break;
			case STARTED:
				cleanDebug("         ");
				printDebug("Finished!");
				measure_State = FINISHED;
				machine_State = SENDING_DATA;
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
