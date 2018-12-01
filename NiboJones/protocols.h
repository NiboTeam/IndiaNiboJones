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
	RUNNING_FORWARD,
	TURNING_LEFT,
	TURNING_RIGHT,
	WAITING,
	END
} machine_state;

extern enum state machine_state;

void init_protocol();
void runForward_protocol();
void leftTurn_protocol();
void rightTurn_protocol();
int obstacleInSight(int sensorID, int thresholdDistanceValue);
#endif
