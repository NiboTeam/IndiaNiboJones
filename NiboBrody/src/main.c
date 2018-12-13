/**
  * @brief This programm receives single segments from the NIBO and creates an outline with these segments.
  * @author Benedikt Petschelt
  * @date 7.12.2018
  * @version 1.0
**/

#include <stdio.h>
#include <stdlib.h>
// Sleep for Linux
#include <unistd.h>
// XBee
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>

// Path to the XBEE modul
#define XBEEPATH "/dev/ttyUSB0"
// Used to only address the correct devices
#define XBEEID 128
// Enlarges the outline, for a better presentation
#define SCALEFACTOR 1
// Represents the corners of a segment
#define CORNERRADIUS 2

// Contains the possible directions the NIBO can take (top down perspective)
typedef enum{
	UP = 0,
    RIGHT = 1,
	DOWN = 2,
    LEFT = 3
} DIRECTIONS;
// A single segment out of the complete outline
typedef struct segment{
	int distance;
	DIRECTIONS direction;
	// Needed to parse the size of the outline and later on to fill it
	struct segment *nextSegment;
	// Needed to correct the length of segments at the end of an outline (rounding errors could lead to startpoint != endpoint)
	struct segment *previousSegment;
} segment;

segment *head = NULL;
segment *end = NULL;
// Contains the complete outline
char *output;
int columns = 0;
int rows = 0;
// These variables contain the startpoint -> needed to correctly fill the array
int startColumn = 0;
int startRow = 0;
// These variables contain the endpoint -> needed to correct possible rounding error
int endRow = 0;
int endColumn = 0;
// Starting direction is upwards -> If a new message is transmitted, the new direction will be set to the right or left side
int currentDirection = 0;
// Is the NIBO currently working?
int isRunning = 0;

// Structures for the XBee communication
struct termios old_termios;
struct termios new_termios;
// Variable to save the incoming message
unsigned char messageBuffer;
// Pointed on the opened serial device
int serialDevice;

/**
  * Appends a new segment to the outline
  * @param distance The length of the new segment
  * @param newDirection The direction of the NIBO. 0 - left and 1 - right
**/
void createSegment(int distance, int newDirection);
/**
  * Parses the size of 'output'
**/
void parseArraySize();
/**
  * Fills the output with white spaces and walls
**/
void fillArray();
/**
  * Prints the current outline
**/
void printOutline();
/**
  * Based on the direction of the NIBO, the concrete direction is calculated
  * @param newDirection The direction of the NIBO. 0 - left and 1 - right
**/
void setDirection(int newDirection);
/**
  * If necessary, corrects the outline so that the structure is complete
**/
void correctOutline();
/**
  * Sets up all needed paramters and data structures for the XBee communication
**/
int initializeXBee();
/**
  * Parses an incoming message from the NIBO
  * @param message The message in form of a byte
**/
int parseMessage(unsigned char message);
/**
  * Sends an acknowledgement to the NIBO, without waiting for an answer
**/
void sendAcknowledgement();
/**
  * Sends a single byte to the NIBO and awaits an answer
  * @param The byte which is gonna by transmitted
**/
int sendMessage(unsigned char byte);
/**
  * Sends the outline to the NIBO at the end of the drive
**/
void sendOutline();


int main(){
	setvbuf(stdout, NULL, _IONBF, 0);
	if(initializeXBee() == -1){
		printf("Initializing failed! Closing program...");
		return -1;
	}
	printf("Waiting for segments...");
	int received = -1;
	while (1 == 1) {
		// Operation sleep
		sleep(0.5);
		received = read(serialDevice, &messageBuffer, 1);
		if(messageBuffer == 0 || received == -1){
			continue;
		}
		if(parseMessage(messageBuffer) == 1){
			// NIBO detected a line on the ground
			if(isRunning == 1){
				// The NIBO finished the outline -> free all resources and correct the outline if necessary
				printf("\n\nCorrecting outline...\n");
				correctOutline();
				parseArraySize();
				fillArray();
				printOutline();

				//printf("\nSending...");
				//sendOutline();
				//printf("\nFinished");

				segment *currentPart = head;
				segment *temp = NULL;
				while (currentPart->nextSegment != NULL){
					temp = currentPart->nextSegment;
					free(currentPart);
					currentPart = temp;
				}
				free(temp);
				head = NULL;
				end = NULL;
				isRunning = 0;
			}else{
				// The NIBO is at the beginning of a new outline, no additional action needed
				isRunning = 1;
			}
		}
		// ensures that the segments do not get read twice
		sleep(2);
		// Clears input buffer of the xbee modul
		tcflush(serialDevice, TCIOFLUSH);
	}
}
void createSegment(int distance, int newDirection){
	setDirection(newDirection);
	// The SCALEFACTOR enlarges the outline, for a better presentation, while the +2 represent the corners
	distance = distance * SCALEFACTOR + CORNERRADIUS;
	if (head == NULL){
		head = malloc(sizeof(segment));
		head->distance = distance;
		head->direction = currentDirection;
		head->nextSegment = NULL;
		head->previousSegment = NULL;
	}else{
		segment *newPart = malloc(sizeof(segment));
		end = newPart;
		newPart->direction = currentDirection;
		newPart->distance = distance;
		newPart->nextSegment = NULL;
		newPart->previousSegment = NULL;

		segment *currentPart = head;
		while (currentPart->nextSegment != NULL){
			currentPart = currentPart->nextSegment;
		}
		newPart->previousSegment = currentPart;
		currentPart->nextSegment = newPart;
	}
	printf("\nPrinting new segment...\n\n");
	parseArraySize();
	fillArray();
	printOutline();
}
void parseArraySize(){
	segment *currentSegment = head;
	// current column and row
	int positionColumn = 0;
	int positionRow = 0;

	// Points to the last column
	int pointerColumnMax = 0;
	// Points to the beginning column
	int pointerColumnMin = 0;
	// Points to the upper row
	int pointerRowMax = 0;
	// Points to the bottom row
	int pointerRowMin = 0;

	while(currentSegment != NULL){
		switch(currentSegment->direction){
		case 0:
			positionRow = positionRow + currentSegment->distance - 1;
			if(positionRow > pointerRowMax){
				pointerRowMax = positionRow;
			}
			break;
		case 1:
			positionColumn = positionColumn + currentSegment->distance - 1;
			if(positionColumn > pointerColumnMax){
				pointerColumnMax = positionColumn;
			}
			break;
		case 2:
			positionRow = positionRow - currentSegment->distance +1;
			if(positionRow < pointerRowMin){
				pointerRowMin = positionRow;
			}
			break;
		case 3:
			positionColumn = positionColumn - currentSegment->distance + 1;
			if(positionColumn < pointerColumnMin){
				pointerColumnMin = positionColumn;
			}
			break;
		}
	    currentSegment = currentSegment->nextSegment;
	}
	// The number of columns can be calculated from the distance between the end points
	columns = pointerColumnMax + (-1*pointerColumnMin) + 1;
	// The same applies to the rows
	rows = pointerRowMax + (-1*pointerRowMin) + 1;
	// The start column depends on how often the NIBO has gone "left"
	startColumn = pointerColumnMin*-1;
	// The start row depends on how often the NIBO has gone "up"
	startRow = pointerRowMax;
}
void fillArray(){
	output = malloc(sizeof(char) * (columns*rows));
	if (output == NULL){
	    printf("ERROR: Could not allocate enough space for the output array.");
	    return;
	}
	// Filling the array with white spaces
	for(int i = 0; i < rows; i++){
		for(int j = 0; j < columns; j++){
			output[j+(i*columns)] = ' ';
		}
	}
	segment *currentSegment = head;
	int pointerColumn = startColumn;
	int pointerRow = startRow;
	while(currentSegment != NULL){
		int i = 0;
		switch(currentSegment->direction){
		case 0:
			for(i = pointerRow; i > (pointerRow-currentSegment->distance); i--){
				output[pointerColumn+(i*columns)] = '#';
			}
			pointerRow = i+1;
			break;
		case 1:
			for(i = pointerColumn; i < (pointerColumn+currentSegment->distance); i++){
				output[i+(pointerRow*columns)] = '#';
			}
			pointerColumn = i-1;
			break;
		case 2:
			for(i = pointerRow; i < (pointerRow+currentSegment->distance); i++){
				output[pointerColumn+(i*columns)] = '#';
			}
			pointerRow = i-1;
			break;
		case 3:
			for(i = pointerColumn; i > (pointerColumn - currentSegment->distance); i--){
				output[i+(pointerRow*columns)] = '#';
			}
			pointerColumn = i+1;
			break;
		}
		currentSegment = currentSegment->nextSegment;
	}
	endRow = pointerRow;
	endColumn = pointerColumn;
}
void setDirection(int newDirection){
	// The Nibo always sends the direction (0 - left, 1 - right) he took
	if(newDirection == 0){
		// 4 is the amount of possible directions
		currentDirection = (currentDirection-1) % 4;
		if(currentDirection < 0){
			// -1 represents left and left is represented by 3
			currentDirection = currentDirection + 4;
		}
	}else{
		currentDirection = (currentDirection+1) % 4;
	}
}
void printOutline(){
	for(int i = 0; i < rows; i++){
		for(int j = 0; j < columns; j++){
			printf("%c", output[j+(i*columns)]);
		}
		printf("\n");
	}
	// The output array is recalculated and filled with each new segment -> can be set to free
	free(output);
}
void correctOutline(){
	int difRows = startRow - endRow;
	segment *currentPart = end;
	while(difRows != 0){
		// If the row is wrong, the segments are shortened or lengthened upwards or downwards
		if(difRows < 0 && currentPart->direction == 0 ){
			currentPart->distance = currentPart->distance + difRows*(-1);
			difRows = 0;
		}else if(difRows > 0 && currentPart->direction == 0 ){
			if(currentPart->distance > difRows){
				currentPart->distance = currentPart->distance - difRows;
				difRows = 0;
			}else{
				currentPart->distance = 1 * SCALEFACTOR;
				difRows = difRows - currentPart->distance + 1;
			}
		}
		currentPart = currentPart->previousSegment;
	}

	int difColumns = startColumn - endColumn;
	currentPart = end;
	// Depending on the direction, the positive and negative values must be processed differently
	while(difColumns != 0){
		if(difColumns > 0 && currentPart->direction == 1){
			// Last part ends before start -> simply add the difference
			currentPart->distance = currentPart->distance + difColumns;
			difColumns = 0;
		}else if (difColumns > 0 && currentPart->direction == 3){
			if(currentPart->distance > difColumns){
				// Last part ends behind the start, and this part is long enough -> shorten the part
				currentPart->distance = currentPart->distance - difColumns;
				difColumns = 0;
			}else{
				// Last part ends behind the start but this part is not long enough -> shorten, if possible
				if(currentPart->distance < (1 * SCALEFACTOR + CORNERRADIUS)){
					// Part is to short
					continue;
				}
				// Part can be shortened
				int tempDistance = currentPart->distance;
				currentPart->distance = 1 * SCALEFACTOR + CORNERRADIUS;
				difColumns = difColumns - (tempDistance - currentPart->distance);
			}
		}else if(difColumns < 0 && currentPart->direction == 1){
			if(currentPart->distance > (difColumns*-1)){
				currentPart->distance = currentPart->distance + difColumns;
				difColumns = 0;
			}else{
				if(currentPart->distance < (1 * SCALEFACTOR + CORNERRADIUS)){
					// Part is to short
					continue;
				}
				int tempDistance = currentPart->distance;
				currentPart->distance = 1 * SCALEFACTOR + CORNERRADIUS;
				difColumns = difColumns - (tempDistance - currentPart->distance);
			}
		}else if(difColumns < 0 && currentPart->direction == 3){
			currentPart->distance = currentPart->distance + difColumns;
			difColumns = 0;
		}
		currentPart = currentPart->previousSegment;
	}
	printf("\nCorrecting finished...\n");
}
int initializeXBee(){
	serialDevice = open(XBEEPATH, O_RDWR | O_NOCTTY | O_NDELAY);
	if (serialDevice < 0) {
		fprintf(stderr, "error, counldn't open file %s\n", XBEEPATH);
		return -1;
	}
	if (tcgetattr(serialDevice, &old_termios) != 0) {
		fprintf(stderr, "tcgetattr(fd, &old_termios) failed: %s\n", strerror(errno));
		return -1;
	}
	memset(&new_termios, 0, sizeof(new_termios));
	new_termios.c_iflag = IGNPAR;
	new_termios.c_oflag = 0;
	new_termios.c_cflag = CS8 | CREAD | CLOCAL | HUPCL;
	new_termios.c_lflag = 0;
	new_termios.c_cc[VINTR]    = 0;
	new_termios.c_cc[VQUIT]    = 0;
	new_termios.c_cc[VERASE]   = 0;
	new_termios.c_cc[VKILL]    = 0;
	new_termios.c_cc[VEOF]     = 4;
	new_termios.c_cc[VTIME]    = 0;
	new_termios.c_cc[VMIN]     = 1;
	new_termios.c_cc[VSWTC]    = 0;
	new_termios.c_cc[VSTART]   = 0;
	new_termios.c_cc[VSTOP]    = 0;
	new_termios.c_cc[VSUSP]    = 0;
	new_termios.c_cc[VEOL]     = 0;
	new_termios.c_cc[VREPRINT] = 0;
	new_termios.c_cc[VDISCARD] = 0;
	new_termios.c_cc[VWERASE]  = 0;
	new_termios.c_cc[VLNEXT]   = 0;
	new_termios.c_cc[VEOL2]    = 0;

	if (cfsetispeed(&new_termios, B9600) != 0) {
		fprintf(stderr, "cfsetispeed(&new_termios, B9600) failed: %s\n", strerror(errno));
		return -1;
	}
	if (cfsetospeed(&new_termios, B9600) != 0) {
		fprintf(stderr, "cfsetospeed(&new_termios, B9600) failed: %s\n", strerror(errno));
		return -1;
	}
	if (tcsetattr(serialDevice, TCSANOW, &new_termios) != 0) {
		fprintf(stderr, "tcsetattr(serialDevice, TCSANOW, &new_termios) failed: %s\n", strerror(errno));
		return -1;
	}
	return 1;
}
int parseMessage(unsigned char message){
	// Converting the message to an integer value
	int value = message;
	/*
	 * We transmit one byte - The first bit (from the left) indicates which device was addressed
	 * (1 is for the primary devices - PC and active NIBO, 0 is for the passive NIBO)
	*/
	int id = value & (1 << 7);
	if(id == XBEEID){
		// The second bit indicates whether a line has been detected on the ground (0 - no, 1 - yes)
		int line = value & (1<<6);
		// The third bit indicates which direction the NIBO took, as he entered the new segment (0 - left, 1 - right)
		int direction = value & (1<<5);
		// The last five bits contain the length of the segment (max. length of 3.1 meters)
		int length = value & 31;
		// The direction has to be set up, before it can be parsed

		if(direction > 0){
			direction = 1;
		}else{
			direction = 0;
		}
		createSegment(length, direction);
		sendAcknowledgement();
		// Returns whether a line was recognized or not
		return (line > 0 ? 1 : -1);
	}else{
		return -1;
	}
}
void sendAcknowledgement(){
	// Content of the actual message is meaningless, its just used for the acknowledgement
	printf("\nAcknowledgement send...\n");
	int byte = 128;
	unsigned char answer = byte & 255;
	write(serialDevice, &answer , 1);
}
int sendMessage(unsigned char byte){
	int counter = 0;
	int received = -1;
	int status = 0;
	while (1 == 1) {
		write(serialDevice, &byte , 1);
		sleep(1);
		received = read(serialDevice, &messageBuffer, 1);
		if(messageBuffer == 0 || received == -1){
			if(counter > 10){
				status = -1;
				break;
			}
			counter++;
			continue;
		}else{
			int answer = messageBuffer;
			if( (answer & 128) == 128){
				status = 1;
				break;
			}
		}
	}
	sleep(1);
	tcflush(serialDevice, TCIOFLUSH);
	return status;
}
void sendOutline(){
	int byte = columns + 128;
	unsigned char message = byte & 255;
	if(sendMessage(message) == -1){
		printf("Failed communication between the NIBO and the PC. Canceling...");
		return;
	}
	byte = rows + 128;
	message = byte & 255;
	if(sendMessage(message) == -1){
		printf("Failed communication between the NIBO and the PC. Canceling...");
		return;
	}
	segment *currentPart = head;
	while(currentPart != NULL){
		int byte = 128 + 32 * (currentPart->direction & 3) + currentPart->distance;
		message = byte & 255;
		if(sendMessage(message) == -1){
			printf("Failed communication between the NIBO and the PC. Canceling...");
			return;
		}
		currentPart = currentPart->nextSegment;
	}
}

