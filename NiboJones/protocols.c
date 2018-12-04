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
#include <nibo/spi.h>
#include "gfxOutput.h"
#include "protocols.h"
#include <nibo/delay.h>

#define FRONT_THRESHOLD 150
#define SIDE_OUTSIDE_TURN_THRESHOLD 20
#define SIDE_THRESHOLD_OUTSIDE_MAX 200
#define SIDE_THRESHOLD_OUTSIDE_MIN 190

enum correctState {
	OUTSIDE = 0,
	INSIDE = 1,
	NON
} correctState;

int lastCorrectState = -1;

int counter = 0;

void init_protocol(){
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
	cleanDebug(17);
	printMovingDirection(run_direction);
}

void correctTrackToLeft(){
	copro_setSpeed(10, 12);
	delay(200);
}

void correctTrackToRight(){
	copro_setSpeed(12, 10);
	delay(200);
}

void disableCorrectionOfTrack(int last_correction, int runDirection){
	if(last_correction == OUTSIDE){
		correctTrackToInside(runDirection);
	}else if(last_correction == INSIDE){
		correctTrackToOutside(runDirection);
	}
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

void leftTurn_protocol() {
	copro_stop();
	copro_setTargetRel(-27, 27, 10);
	delay(2000);
}
void rightTurn_protocol() {
	copro_stop();
	copro_setTargetRel(27, -27, 10);
	delay(2000);
}

void stayOnTheRightTrack(int runDirection){
	copro_update();
	int lastCorrection = correctState;
	if(copro_distance[runDirection] / 256 > SIDE_THRESHOLD_OUTSIDE_MAX){
		correctState = INSIDE;
		correctTrackToInside(runDirection);
	}else if(copro_distance[runDirection] / 256 < SIDE_THRESHOLD_OUTSIDE_MIN) {
		correctState = OUTSIDE;
		correctTrackToOutside(runDirection);
	}else{
		correctState = NON;
		disableCorrectionOfTrack(lastCorrection, runDirection);
	}

	if(lastCorrectState != correctState){
		cleanDebug(15);
		if(correctState == INSIDE){
			printDebug("Correct Outside!");
		} else if(correctState == OUTSIDE){
			printDebug("Correct Inside!");
		} else{
			printDebug("No correction!");
		}
	}
}

void runForward_protocol(){
	copro_update();
	counter++;
	int front = copro_distance[2] / 256;
	int side_outside = copro_distance[run_direction] / 256;

	/*if(counter % 800 == 0 || (counter % 800) - 1 == 0 || (counter % 800) + 1 == 0){
		printInfo("                        ", 40);
		printInfo("                        ", 47);
		char output[20];
		sprintf(output, "Front: %3i ", front);
		printInfo(output, 40);
		sprintf(output, "Side: %3i ", side_outside);
		printInfo(output, 47);
	}*/

	if (front >= FRONT_THRESHOLD) { //vorne Wand gefunden -> Innenkurve
		if(run_direction == LEFT_RUN){
			machine_state = TURNING_LEFT;
		}else{
			machine_state = TURNING_RIGHT;
		}
	} else if(run_direction != UNKNOWN && side_outside < SIDE_OUTSIDE_TURN_THRESHOLD){ // Seitenwand weg -> Außenkurve
		//copro_setTargetRel(15, 15, 10);
		delay(2000);
		if(run_direction == LEFT_RUN){
			machine_state = TURNING_RIGHT;
		}else{
			machine_state = TURNING_LEFT;
		}
	} else { //freie Bahn
		copro_setSpeed(10, 10);
		stayOnTheRightTrack(run_direction);
	}
}
