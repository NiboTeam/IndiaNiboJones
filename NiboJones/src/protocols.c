/**
 @file protocols.c
 @author H.Tanke
 @version 1.0
 @brief Holds different protocols and util-methods for controlling nibo
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
/*#define SIDE_THRESHOLD_OUTSIDE_MAX 200
 #define SIDE_THRESHOLD_OUTSIDE_MIN 190
 #define BLADE_THRESHOLD_OUTSIDE_MAX 100
 #define BLADE_THRESHOLD_OUTSIDE_MIN 50*/
#define BLACK_UNDERGROUND 40

enum correctState {
	OUTSIDE = 0, INSIDE = 1, NON
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
int segmentWidth;
int segmentHeight;

int counter = 0;

/**
 * Setter for acccess from main file. Sets whether a
 * black line on the ground was recognized.
 * @param reconized
 **/
void setRecognizedBlackLine(int reconized) {
	reconizedBlackLine_send = reconized;
}

/**
 * Getter for access from main file. Returns whether a black line on the floor was recognized.
 * @return value whether a black line was recognized
 **/
int getRecognizedBlackLine() {
	return reconizedBlackLine_send;
}

/**
 * Getter for access from main file. Returns the last turn direction.
 * @return last turn direction (0 = left, 1 = right)
 **/
int getTurnDirection() {
	return turnDirection_send;
}

/**
 * Sets and calculates start values for right positioning of outline on display.
 * @param columns numbers columns the outline pictures has (width)
 * @param rows numbers columns the outline pictures has (height)
 * @param coordx x coordinate of the start point for painting
 * @param coordy y coordinate of the start point for painting
 **/
void setStartValues(int columns, int rows, int coordx, int coordy) {
	//by using 50 the outline will be placed more on the upper boarder of display.
	//In this way, below the outline is place for a debug outprint with further informations
	segmentWidth = 50 / columns;
	segmentHeight = 50 / rows;
	//startx coordinate will be manipulated so the whole outline is placed in the middle of the display.
	int startx = (segmentWidth * coordx) + ((128 - segmentWidth * columns) / 2);
	int starty = (segmentHeight * coordy) + 5;
	setStartPointForOutline(startx, starty);
}

/**
 * Sometimes the buffer of XBee communication will be filled randomly.
 * This will be wiped to provide a better receiving from NIBOBrody.
 **/
void clearBuffer(){
	while(!uart0_rxempty()){
		unsigned char received = uart0_getchar();
	}
}

/**
 * The message which contains parts of the start values are slightly different
 * and need a own function. Because sometimes xbee messages are failing,
 * this function listens for several seconds.
 * @return returns whether there was received a message in time.
 **/
int receiveStartValues() {
	int byte;
	unsigned char received;
	for (int i = 0; i < 20; i++) {
		if (!uart0_rxempty()) {
			received = uart0_getchar();
			byte = received;
			if ((byte & 128) == 128) {
				//sending acknowledgment
				unsigned char answer = byte & 255;
				if (!uart0_txfull()) {
					uart0_putchar(answer);
					delay(200);
				}
				return byte & 127;
			}
		}
		delay(200);
	}

	return -1;
}

/**
 * The message which contains parts of the outline figur are slightly different
 * and need a own function. Because sometimes xbee messages are failing,
 * this function listens for several seconds until closing.
 * @return returns whether there was received a message in time.
 **/
int receiveOutline() {
	int byte;

	unsigned char received;
	for (int i = 0; i < 10; i++) {
		if (!uart0_rxempty()) {
			received = uart0_getchar();
			byte = received;
			if ((byte & 128) == 128) {
				unsigned char answer = byte & 255;
				if (!uart0_txfull()) {
					uart0_putchar(answer);
					delay(200);
				}
				return byte;
			}
		}
		delay(400);
	}
	return -1;
}

/**
 * This message sends a signal to NIBOShorty so it can start playing music and showing picture.
 * @param signal sets start signal (1 = start, 0 = stop)
 */
void sendMessageToNibo(int signal) {
	// signal indicates the start or end of the music
	// this message addresses the NIBO, so the indication bit has to be 0
	int byte = signal;
	unsigned char message = byte & 255;
	unsigned char answer;
	int i = 0;
	if (!uart0_txfull()) {
		uart0_putchar(message);
		delay(200);
		cleanDebug(15);
		char output[20];
		sprintf(output, "Send %i to nibo!", signal);
		printDebug(output);
	}
	/*while (1) {


		if (!uart0_rxempty()) {
			// wait for answer
			answer = uart0_getchar();
			byte = answer;
			if ((byte & 128) != 0) {
				// wrong indicator
				continue;
			}
			break;
		} else if (i == 5) {
			break;
		}
		i++;
	*/
}

/**
 * This protocol initializes NIBOJones. Its measuring distance to wall next to the nibo
 * to set distance which shall be hold to the wall by moving around. Recognizing the movement direction
 * part of this protocol too. There are two ways of moving direction, left and right run.
 */
void init_protocol() {
	sendMessageToNibo(1);
	floor_init();
	copro_update();
	printDebug("Initialization...");
	printMovingDirection(run_direction);
	int averageDistance_SideLeft = 0;
	int averageDistance_SideRight = 0;
	int averageDistance_BladeLeft = 0;
	int averageDistance_BladeRight = 0;

	for (int i = 0; i < 5; i++) {
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

	if (averageDistance_SideLeft > averageDistance_SideRight) {
		run_direction = RIGHT_RUN;
		SIDE_THRESHOLD_OUTSIDE_MAX = averageDistance_SideLeft;
		SIDE_THRESHOLD_OUTSIDE_MIN = averageDistance_SideLeft - 20;
		BLADE_THRESHOLD_OUTSIDE_MAX = averageDistance_BladeLeft;
		BLADE_THRESHOLD_OUTSIDE_MIN = averageDistance_BladeLeft - 20;
	} else {
		run_direction = LEFT_RUN;
		SIDE_THRESHOLD_OUTSIDE_MAX = averageDistance_SideRight + 10;
		SIDE_THRESHOLD_OUTSIDE_MIN = averageDistance_SideRight - 10;
		BLADE_THRESHOLD_OUTSIDE_MAX = averageDistance_BladeRight + 10;
		BLADE_THRESHOLD_OUTSIDE_MIN = averageDistance_BladeRight - 10;
	}

	FRONT_THRESHOLD = SIDE_THRESHOLD_OUTSIDE_MAX;

	if (SIDE_THRESHOLD_OUTSIDE_MAX > 250) {
		FRONT_THRESHOLD = FRONT_THRESHOLD - 30;
	} else if (SIDE_THRESHOLD_OUTSIDE_MAX > 230) {
		FRONT_THRESHOLD = FRONT_THRESHOLD - 10;
	}

	if (run_direction == LEFT_RUN) {
		sensorIDSide = 0;
		sensorIDBlade = 1;
		turnDirection_send = 0;
	} else {
		sensorIDSide = 4;
		sensorIDBlade = 3;
		turnDirection_send = 1;
	}

	cleanDebug(17);
	printMovingDirection(run_direction);
}

/**
 * Instructs the nibo to correct its track to left. This helps nibo to follow the wall.
 */
void correctTrackToLeft() {
	leds_set_status(LEDS_ORANGE, 4);
	leds_set_status(LEDS_OFF, 5);
	copro_setSpeed(10, 12);
}

/**
 * Instructs the nibo to correct its track to right. This helps NIBO to follow the wall.
 */
void correctTrackToRight() {
	leds_set_status(LEDS_ORANGE, 5);
	leds_set_status(LEDS_OFF, 4);
	copro_setSpeed(12, 10);
}

/**
 * Because there are 2 ways to move around the structure this method decides
 * in which way the track will be corrected. To inside means to the center of structure.
 * @param direction run direction in which the NIBO moves around structure
 */
void correctTrackToInside(int direction) {
	if (direction == LEFT_RUN) {
		correctTrackToLeft();
	} else if (direction == RIGHT_RUN) {
		correctTrackToRight();
	}
}

/**
 * Because there are 2 ways to move around the structure this method decides
 * in which way the track will be corrected. To outside means away from center of structure.
 * @param direction run direction in which the NIBO moves around structure
 */
void correctTrackToOutside(int direction) {
	if (direction == LEFT_RUN) {
		correctTrackToRight();
	} else if (direction == RIGHT_RUN) {
		correctTrackToLeft();
	}
}

/**
 * With this method the NIBO is able to turn left at a corner as inside turn.
 * Because the NIBO is not able to perform perfect 90° turns its orientating on the corner and its wall
 * to perform its turn. Those wall and corner have to be positioned as 90° corner!
 */
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

/**
 * For outside turns the NIBOs looses contact to walls and is free in field.
 * In this situation the nibo has to perform a 90° turn of its own.
 * Use this method to perform a right turn.
 */
void leftTurnForced() {
	turnDirection_send = 0;
	leds_set_status(LEDS_ORANGE, 3);
	copro_setTargetRel(-27, 27, 10);
	delay(2000);
	leds_set_status(LEDS_OFF, 3);
}

/**
 * With this method the NIBO is able to turn right at a corner as inside turn.
 * Because the NIBO is not able to perform perfect 90° turns its orientating on the corner and its wall
 * to perform its turn. Those wall and corner have to be positioned as 90° corner!
 */
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

/**
 * For outside turns the NIBOs looses contact to walls and is free in field.
 * In this situation the nibo has to perform a 90° turn of its own.
 * Use this method to perform a left turn.
 */
void rightTurnForced() {
	turnDirection_send = 1;
	leds_set_status(LEDS_ORANGE, 6);
	copro_setTargetRel(27, -27, 10);
	delay(2000);
	leds_set_status(LEDS_OFF, 6);
}

/**
 * There are different ways for NIBOJones to run around the structure.
 * This method decides whether left or right shall be choosen to turn to inside of structure.
 */
void turnInside_protocol() {
	if (run_direction == LEFT_RUN) {
		leftTurnFree();
	} else {
		rightTurnFree();
	}
}

/**
 * There are different ways for NIBOJones to run around the structure.
 * This method decides whether left or right shall be choosen to turn to outside of structure.
 */
void turnOutside_protocol() {
	if (run_direction == LEFT_RUN) {
		rightTurnForced();
	} else {
		leftTurnForced();
	}
	delay(1000);
}

/**
 * This protocol corrects the track of NIBO running forward
 * to prevent collision with wall on the sides. Its correcting to inside or outside.
 * In case of specific run direction it a correction to left or right
 */
void trackCorrection_protocol() {
	copro_update();
	//if side sensors value hits threshold of nearest distance to wall.
	if (copro_distance[sensorIDSide] / 256 > SIDE_THRESHOLD_OUTSIDE_MAX
			|| copro_distance[sensorIDBlade] / 256
					> BLADE_THRESHOLD_OUTSIDE_MAX) {
		correctState = INSIDE;
		correctTrackToInside(run_direction);
	//if side sensor value hits threshold of most far distance to wall.
	} else if (copro_distance[sensorIDSide] / 256 < SIDE_THRESHOLD_OUTSIDE_MIN
			|| copro_distance[sensorIDBlade] / 256
					< BLADE_THRESHOLD_OUTSIDE_MIN) {
		correctState = OUTSIDE;
		correctTrackToOutside(run_direction);
	} else {
		correctState = NON;
		leds_set_status(LEDS_OFF, 4);
		leds_set_status(LEDS_OFF, 5);
		copro_setSpeed(10, 10);
	}

	lastCorrectState = correctState;
	machine_State = RUNNING_FORWARD;
}

/**
 * If the NIBO crosses a black line by moving around the structure with this method it will be noticed.
 * After recognition this method will change the machine state to next state.
 * It will start or stop the measuring process too.
 * @return returns whether the floor sensors noticed a black line.
 */
int floorCheck() {
	floor_update();

	if (reconizedBlackLine_send == 0
			&& (floor_relative[FLOOR_LEFT] <= BLACK_UNDERGROUND
					|| floor_relative[FLOOR_RIGHT] <= BLACK_UNDERGROUND)) {
		switch (measure_State) {
		case FINISHED:
			cleanDebug("        ");
			printDebug("Started!");
			copro_resetOdometry(0, 0);
			measure_State = STARTED;
			copro_stop();
			sendMessageToNibo(1);
			//delay(1000);
			//sendMessageToNibo(1);
			break;
		case STARTED:
			cleanDebug("         ");
			printDebug("Finished!");
			measure_State = FINISHED;
			machine_State = SENDING_DATA;
			copro_stop();
			sendMessageToNibo(0);
			sendMessageToNibo(0);
			break;
		}
		reconizedBlackLine_send = 1;
		return 1;
	}
	return 0;
}

/**
 * This protocol will paint the outline of moved around and measured structure on NIBOs display.
 * Receiving data from NIBOBrody for painting is also part of this protocol.
 * @return returns whether all data for paining was received successfully.
 */
int showOutline_protocol() {
	printDebug("Start receiving");
	int columns = receiveStartValues();
	if (columns == -1) {
		cleanDebug(20);
		printDebug("ERROR: columns");
		return -1;
	}
	int rows = receiveStartValues();
	if (columns == -1) {
		cleanDebug(20);
		printDebug("ERROR: rows");
		return -1;
	}
	int startX = receiveStartValues();
	if (columns == -1) {
		cleanDebug(20);
		printDebug("ERROR: startx");
		return -1;
	}
	int startY = receiveStartValues();
	if (columns == -1) {
		cleanDebug(20);
		printDebug("ERROR: starty");
		return -1;
	}
	cleanDebug(20);
	printDebug("connect successful");
	setStartValues(columns, rows, startX, startY);

	int isFinished = 0;
	int byte = -1;

	while (!isFinished) {
		isFinished = 1;
		byte = receiveOutline();
		if (byte != -1) {
			showOutline(byte, segmentHeight, segmentWidth);
			isFinished = 0;
			break;
		} else {
			delay(200);
		}
		byte = -1;
	}

	return 1;
}

/**
 * This protocol will be started at the end of the measuring run around the structure.
 * Its starting outline painting protocol.
 */
void finishRun_protocol() {
	//sometimes buffer is filled before received any message from NIBOBrody. This could cause problems.
	clearBuffer();
	for (int i = 0; i < 5; i++) {
		//if showOutline protocol will fail it will be repeated several times.
		if (showOutline_protocol() == 1) {
			break;
		}
		delay(500);
	}
	cleanDebug(16);
	printDebug("End of communication!");
}

/**
 * This protocol controls the NIBO by running forward. Its the main protocol too and
 * starts all others protcolls by setting machine state.
 */
void runForward_protocol() {
	copro_update();
	int front = copro_distance[2] / 256;
	int side_outside = copro_distance[run_direction] / 256;

	if (s3_was_pressed()) {
		machine_State = WAITING;
		return;
	}

	//if a black line was recognized this will perform a hard switch of machine state to improve reaction time.
	if (floorCheck() == 1) {
		//machine_State = WAITING;
		return;
	}

	if (front >= FRONT_THRESHOLD) { //wall on front
		copro_stop();
		machine_State = TURNING_INSIDE;
	} else if (run_direction
			!= UNKNOWN&& side_outside < SIDE_OUTSIDE_TURN_THRESHOLD) { // loosed contact to wall on side
		machine_State = TURNING_OUTSIDE;
		delay(1000);
		copro_stop();
	} else { //no wall on front or loosed contact to wall on side
		machine_State = TRACK_CORRECTION;
	}
}
