/*
 ============================================================================
 Name        : protocols.h
 Author      :
 Version     :
 Description : Holds all a bunch of moving protocols for nibo.
 ============================================================================
 */
#ifndef PROTOCOLS_H_
#define PROTOCOLS_H_

enum direction {
	LEFT_RUN = 0,
	UNKNOWN = 2,
	RIGHT_RUN = 4
} run_direction;

enum state {
	INITIALIZATION,
	TRACK_CORRECTION,
	TURNING_INSIDE,
	TURNING_OUTSIDE,
	RUNNING_FORWARD,
	SENDING_DATA,
	WAITING,
	FINISHING_RUN
} machine_State;

enum measurement {
	FINISHED = 0,
	STARTED = 1
} measure_State;

/**
 * Setter for acccess from main file. Sets whether a
 * black line on the ground was recognized.
 * @param recognized
 **/
void setRecognizedBlackLine(int recognized);
/**
 * Getter for access from main file. Returns whether a black line on the floor was recognized.
 * @return value whether a black line was recognized
 **/
int getRecognizedBlackLine();
/**
 * Getter for access from main file. Returns the last turn direction.
 * @return last turn direction (0 = left, 1 = right)
 **/
int getTurnDirection();
/**
 * This protocol initializes NIBOJones. Its measuring distance to wall next to the nibo
 * to set distance which shall be hold to the wall by moving around. Recognizing the movement direction
 * part of this protocol too. There are two ways of moving direction, left and right run.
 */
void init_protocol();
/**
 * This protocol controls the NIBO by running forward. Its the main protocol too and
 * starts all others protcolls by setting machine state.
 */
void runForward_protocol();
/**
 * There are different ways for NIBOJones to run around the structure.
 * This method decides whether left or right shall be choosen to turn to inside of structure.
 */
void turnInside_protocol();
/**
 * There are different ways for NIBOJones to run around the structure.
 * This method decides whether left or right shall be choosen to turn to outside of structure.
 */
void turnOutside_protocol();
/**
 * This protocol corrects the track of NIBO running forward
 * to prevent collision with wall on the sides. Its correcting to inside or outside.
 * In case of specific run direction it a correction to left or right
 */
void trackCorrection_protocol();
/**
 * This protocol will be started at the end of the measuring run around the structure.
 * Its starting outline painting protocol.
 */
void finishRun_protocol();
#endif
