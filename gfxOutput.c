/*
 ============================================================================
 Name        : protocols.h
 Author      :
 Version     :
 Description : Holds all a bunch of methods to display state alerts on nibo display.
 ============================================================================
 */

#include <nibo/gfx.h>
// Wartefunktionen
#include <nibo/delay.h>

void initDisplay(){
	display_init();
	gfx_init();
}

void printDebug(char array[40]){
	gfx_print_text("                                        ");
	gfx_move(50, 0);
	gfx_print_text(array);
}

void printMachineState(int state) {
	char output[20];
	gfx_move(0, 5);
	gfx_print_text("Machine state:");
	gfx_move(0, 15);

	switch (state) {
	case 0:
		sprintf(output, "RUNNING_FORWARD");
		break;
	case 1:
		sprintf(output, "TURNING_LEFT");
		break;
	case 2:
		sprintf(output, "TURNING_RIGHT");
		break;
	case 3:
		sprintf(output, "WAITING");
		break;
	case 4:
		sprintf(output, "RUNNING_BACKWARDS");
		break;
	case 5:
		sprintf(output, "HARD_LEFT");
		break;
	case 6:
		sprintf(output, "HARD_RIGHT");
		break;
	}

	gfx_print_text(output);

	if (state == 3) {
		gfx_move(0, 30);
		gfx_print_text("press s3 for start");
	}
}
