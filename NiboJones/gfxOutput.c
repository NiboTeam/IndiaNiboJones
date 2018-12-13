/*
 ============================================================================
 Name        : gfxOutput.c
 Author      :
 Version     :
 Description : Holds all a bunch of methods to display state alerts on nibo display.
 ============================================================================
 */
#include <stdio.h>
#include <nibo/gfx.h>
// Wartefunktionen
#include <nibo/delay.h>

char oldDebug[];

void initDisplay(){
	display_init();
	gfx_init();
}

void cleanDebug(int length){
	gfx_move(0, 55);
	char whiper[length];

	for(int i = 0; i < length; i++){
		whiper[i] = ' ';
	}

	gfx_print_text(whiper);
}

void printDebug(char array[40]){
	gfx_move(0, 55);
	gfx_print_text(array);
}

void printInfo(char array[40], int height){
	gfx_move(0, height);
	gfx_print_text(array);
}

void printMovingDirection(int direction){
	gfx_move(120, 55);
	gfx_print_text("  ");
	gfx_move(120, 55);

	char output[2];
	switch (direction) {
	case 0:
		gfx_print_text("L");
		break;
	case 3:
		gfx_print_text("U");
		break;
	case 6:
		gfx_print_text("R");
		break;
	}
}

void printMachineState(int state) {
	char output[20];
	gfx_move(0, 5);
	gfx_print_text("Machine state:");
	gfx_move(0, 15);

	gfx_print_text("                        ");
	gfx_move(0, 15);

	switch (state) {
	case 0:
		sprintf(output, "INITIALIZATION");
		break;
	case 1:
		sprintf(output, "TRACK_CORRECTION");
		break;
	case 2:
		sprintf(output, "TURNING_INSIDE");
		break;
	case 3:
		sprintf(output, "TURNING_OUTSIDE");
		break;
	case 4:
		sprintf(output, "RUNNING_FORWARD");
		break;
	case 5:
		sprintf(output, "SENDING_DATA");
		break;
	case 6:
		sprintf(output, "WAITING");
		break;
	case 7:
		sprintf(output, "FINISHING_RUN");
		break;
	}

	gfx_print_text(output);

	if (state == 6) {
		gfx_move(0, 30);
		gfx_print_text("press s3 for start");
	}else{
		gfx_move(0, 30);
		gfx_print_text("                        ");
	}
}
