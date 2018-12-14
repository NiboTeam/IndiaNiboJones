/**
 @file gfxOutput.c
 @author H.Tanke
 @version 1.0
 @brief Holds all a bunch of methods to display state alerts on nibo display.
 */
#include <stdio.h>
#include <nibo/gfx.h>
// Wartefunktionen
#include <nibo/delay.h>
#include <nibo/copro.h>
// Kommunikationsfunktionen fuer die SPI Schnittstelle zum Coprozessor
#include <nibo/spi.h>

//easy control to enable or disable different outputs on display of NIBO
int showDebug = 1;
int showMachineStages = 1;
int showMovingDirection = 1;
int showInfo = 0;

/**
 * before Display can be used for output is has to initialized.
 */
void initDisplay(){
	display_init();
	gfx_init();
}

/**
 * This will set the start point of outline painting so all looks convenient.
 * @param startX x coordinate outline painting shall start
 * @param startY y coordinate outline painting shall start
 */
void setStartPointForOutline(int startX, int startY){
	gfx_fill(0);
	gfx_move(startX, startY);
}

/**
 * Paints a part of the outline of structure as black line on display.
 * @param byte holds all informations like length or direction of the part.
 * @param segmentHeight is the length of a height segment. A segment is a specific number of pixels.
 * This depends on the total height of the structure.
 * @param segmentWidth is the length of a width segment. A segment is a specific number of pixels.
 * This depends on the total width of the structure.
 */
void showOutline(int byte, int segmentHeight, int segmentWidth) {
	int posx = gfx_get_x();
	int posy = gfx_get_y();
	int direction= byte & 96;
	int length = byte & 31;

	switch (direction) {

	case 0: //oben
		gfx_lineTo(posx, posy - (length * segmentHeight));
		break;
	case 32: //rechts
		gfx_hline(segmentWidth * length);
		break;
	case 64: //unten
		gfx_vline(segmentHeight * length);
		break;
	case 96: //links
		gfx_lineTo(posx - (length * segmentWidth), posy);
		break;

	}

}

/**
 * Cleans display on the bottom part which is reserved as debug output line.
 * @param length number of characters which shall be removed from debug output line.
 */
void cleanDebug(int length){
	if(showDebug != 1){
		return;
	}
	gfx_move(0, 55);
	char whiper[length];

	for(int i = 0; i < length; i++){
		whiper[i] = ' ';
	}

	gfx_print_text(whiper);
}

/**
 * Prints a debug output on the bottom part of the display which is reserved as debug output line.
 * @param array array of chars which shall be printed out on line.
 */
void printDebug(char array[20]){
	if(showDebug != 1){
		return;
	}
	gfx_move(0, 55);
	gfx_print_text(array);
}

/**
 * Prints a info output on the display in debug running mode of project.
 * @param array array of chars which shall placed on display
 * @param height height on the display where info output shall appear.
 */
void printInfo(char array[40], int height){
	if(showInfo != 1){
		return;
	}
	gfx_move(0, height);
	gfx_print_text(array);
}

/**
 * Prints a char on right part of the debug output line which indicates whether the NIBO is in left or right running
 * mode around the structure.
 * @param direction running direction around the structure
 */
void printMovingDirection(int direction){
	if(showMovingDirection != 1){
		return;
	}
	gfx_move(120, 55);
	gfx_print_text("  ");
	gfx_move(120, 55);

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

/**
 * The middle or main part of the display of NIBO is reserved for machine state out prints.
 * This method will print a convenient phrase for the current machine state.
 * @param state current machine state
 */
void printMachineState(int state) {
	if(showMachineStages != 1){
		return;
	}
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
		//cleanDebug(20);
	}else{
		gfx_move(0, 30);
		gfx_print_text("                        ");
	}
}
