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

void init_protocol();
void runForward_protocol();
void turnInside_protocol();
void turnOutside_protocol();
void trackCorrection_protocol();
void finishRun_protocol();
#endif
