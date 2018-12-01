/*
 ============================================================================
 Name        : gfxOutput.h
 Author      :
 Version     :
 Description : Holds all a bunch of methods to display state alerts on nibo display.
 ============================================================================
 */
#ifndef GFXOUTPUT_H_
#define GFXOUTPUT_H_

void initDisplay();
void printDebug(char array[40]);
void printInfo(char array[40], int height);
void cleanDebug();
void printMovingDirection(int direction);
void printMachineState(int state);

#endif
