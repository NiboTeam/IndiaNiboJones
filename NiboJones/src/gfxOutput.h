/**
 @file gfxOutput.h
 @author H.Tanke
 @version
 @brief Holds all a bunch of methods to display state alerts and debugs on NIBO display.
 */
#ifndef GFXOUTPUT_H_
#define GFXOUTPUT_H_

/**
 * before Display can be used for output is has to initialized.
 */
void initDisplay();
/**
 * Prints a debug output on the bottom part of the display which is reserved as debug output line.
 * @param array array of chars which shall be printed out on line.
 */
void printDebug(char array[40]);
/**
 * Prints a info output on the display in debug running mode of project.
 * @param array array of chars which shall placed on display
 * @param height height on the display where info output shall appear.
 */
void printInfo(char array[40], int height);
/**
 * Cleans display on the bottom part which is reserved as debug output line.
 * @param length number of characters which shall be removed from debug output line.
 */
void cleanDebug();
/**
 * Prints a char on right part of the debug output line which indicates whether the NIBO is in left or right running
 * mode around the structure.
 * @param direction running direction around the structure
 */
void printMovingDirection(int direction);
/**
 * The middle or main part of the display of NIBO is reserved for machine state out prints.
 * This method will print a convenient phrase for the current machine state.
 * @param state current machine state
 */
void printMachineState(int state);
/**
 * This will set the start point of outline painting so all looks convenient.
 * @param startX x coordinate outline painting shall start
 * @param startY y coordinate outline painting shall start
 */
void setStartPointForOutline(int startX, int startY);
/**
 * Paints a part of the outline of structure as black line on display.
 * @param byte holds all informations like length or direction of the part.
 * @param segmentHeight is the length of a height segment. A segment is a specific number of pixels.
 * This depends on the total height of the structure.
 * @param segmentWidth is the length of a width segment. A segment is a specific number of pixels.
 * This depends on the total width of the structure.
 */
void showOutline(int byte, int segmentHeight, int segmentWidth);

#endif
