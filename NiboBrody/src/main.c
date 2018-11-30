#include <stdio.h>
#include <stdlib.h>

#include <unistd.h> 					// Sleep for Linux

// Includes for the XBee communication
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>

#define XBEEPATH "/dev/ttyUSB0"

typedef enum{							// Contains the possible directions the Nibo can take (top down perspective)
	UP = 0,
    RIGHT = 1,
	DOWN = 2,
    LEFT = 3
} DIRECTIONS;
typedef struct segment{					// A single segment out of the complete outline
	int distance;
	DIRECTIONS direction;
	struct segment *nextSegment;
}segment;

segment *head = NULL;					// Points to the starting segment
char *output;							// Contains the outline
int columns = 0;						// Defines the amount of columns
int rows = 0;							// Defines the amount of rows
int startColumn = 0;					// Defines the starting column
int startRow = 0;						// Defines the starting row
int currentDirection = 0;

struct termios old_termios;				// Structures for the XBee communication
struct termios new_termios;
unsigned char messageBuffer;					// Buffer to save the chars
int serialDevice;						// Pointed on the opened serial device

void createSegment(int distance, int newDirection);	// Appends a new segment to the outline

void parseArraySize();					// Parses the size of 'output'

void fillArray();						// Fills the output with white spaces and walls

void printOutline();					// Prints the current outline

void setDirection(int newDirection);	// The Nibo always sends his direction (0 - left, 1 - right). From this the new direction must be determined

int initializeXBee();

int parseMessage(unsigned char message);

void sendMessage();


int main(){
	setvbuf(stdout, NULL, _IONBF, 0);
	if(initializeXBee() == -1){
		printf("Initializing failed! Closing program...");
		return -1;
	}
	int finished = -1;
	while (1 == 1) {
		sleep(2);									// needed for the flush
		tcflush(serialDevice, TCIOFLUSH);			// clears input buffer of xbee
		read(serialDevice, &messageBuffer, 1);		// waits for new message
		finished = parseMessage(messageBuffer);

		printf("\n\nNEW SEGMENT:\n");
		parseArraySize();
		fillArray();
		printOutline();
		if(finished == 1){								// NIBO finished the outline, clear used ressources
			segment *currentPart = head;
			while (currentPart->nextSegment != NULL){
				segment *temp = currentPart->nextSegment;
				free(currentPart);
				currentPart = temp;
			}
			head = NULL;
			finished = -1;
		}
	}
}


int initializeXBee(){
	serialDevice = open(XBEEPATH, O_RDWR | O_NOCTTY);	// Opens the serial port
	if (serialDevice < 0) {								// If the selected serial port is not found
		fprintf(stderr, "error, counldn't open file %s\n", XBEEPATH);
		return -1;
	}
	if (tcgetattr(serialDevice, &old_termios) != 0) {	// Returns an integer that either indicates success or failure of the prot opening
		fprintf(stderr, "tcgetattr(fd, &old_termios) failed: %s\n", strerror(errno));
		return -1;
	}

	memset(&new_termios, 0, sizeof(new_termios));
	new_termios.c_iflag = IGNPAR; 						// Ignore parity bit
	new_termios.c_oflag = 0;
	new_termios.c_cflag = CS8 | CREAD | CLOCAL | HUPCL; // Sets the serial port to 8N1
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

	if (cfsetispeed(&new_termios, B9600) != 0) {			// Sets the input speed of the serial interdace to 9600baud
		fprintf(stderr, "cfsetispeed(&new_termios, B9600) failed: %s\n", strerror(errno));
		return -1;
	}
	if (cfsetospeed(&new_termios, B9600) != 0) {			// Sets the output speed of the serial interdace to 9600baud
		fprintf(stderr, "cfsetospeed(&new_termios, B9600) failed: %s\n", strerror(errno));
		return -1;
	}
	if (tcsetattr(serialDevice, TCSANOW, &new_termios) != 0) {		// Sets the termios struct of the file handle fd from the options defined via the options
		fprintf(stderr, "tcsetattr(serialDevice, TCSANOW, &new_termios) failed: %s\n", strerror(errno));
		return -1;
	}
	return 1;
}

void createSegment(int distance, int newDirection){
	setDirection(newDirection);
	distance = distance * 3;			// Enlarges the outline, for a better presentation
	if (head == NULL){
		head = malloc(sizeof(segment));
		head->distance = distance;
		head->direction = currentDirection;
		head->nextSegment = NULL;
	}else{
		segment *newPart = malloc(sizeof(segment));
		newPart->direction = currentDirection;
		newPart->distance = distance;
		newPart->nextSegment = NULL;

		segment *currentPart = head;
		while (currentPart->nextSegment != NULL){
			currentPart = currentPart->nextSegment;
		}
	currentPart->nextSegment = newPart;
	}
}

void parseArraySize(){
	segment *currentSegment = head;

	int positionColumn = 0;			// current column
	int positionRow = 0;			// current row

	int pointerColumnMax = 0;		// points to the right end
	int pointerColumnMin = 0;		// points to the left end
	int pointerRowMax = 0;			// points to the top end
	int pointerRowMin = 0;			// points to the down end

	while(currentSegment != NULL){
		switch(currentSegment->direction){
		case 0:
			positionRow += currentSegment->distance;
			if(positionRow > pointerRowMax){
				pointerRowMax = positionRow;
			}
			break;
		case 1:
			positionColumn += currentSegment->distance;
			if(positionColumn > pointerColumnMax){
				pointerColumnMax = positionColumn;
			}
			break;
		case 2:
			positionRow -= currentSegment->distance;
			if(positionRow < pointerRowMin){
				pointerRowMin = positionRow;
			}
			break;
		case 3:
			positionColumn -= currentSegment->distance;
			if(positionColumn < pointerColumnMin){
				pointerColumnMin = positionColumn;
			}
			break;
		}
	    currentSegment = currentSegment->nextSegment;
	}
	columns = pointerColumnMax + (-1*pointerColumnMin) + 1;	// Distanz zwischen den Extremwerten
	rows = pointerRowMax + (-1*pointerRowMin) + 1;			// Distanz zwischen den Extremwerten
	startColumn = pointerColumnMin*-1;						// abhängig davon, wie oft nach links gegangen wurde
	startRow = pointerRowMax;								// abhängig davon, wie oft nach oben gegangen wurde
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
		switch(currentSegment->direction){
		case 0:
			for(int i = pointerRow; i > (pointerRow-currentSegment->distance); i--){
				output[pointerColumn+(i*columns)] = '#';
			}
			pointerRow = pointerRow - currentSegment->distance;
			break;
		case 1:
			for(int i = 0; i < currentSegment->distance; i++){
				output[pointerColumn+i+(pointerRow*columns)] = '#';
			}
			pointerColumn = pointerColumn + currentSegment->distance;
			break;
		case 2:
			for(int i = pointerRow; i < (pointerRow+currentSegment->distance); i++){
				output[pointerColumn+(i*columns)] = '#';
			}
			pointerRow = pointerRow + currentSegment->distance;
			break;
		case 3:
			for(int i = 0; i < currentSegment->distance; i++){
				output[pointerColumn-i+(pointerRow*columns)] = '#';
			}
			pointerColumn = pointerColumn - currentSegment->distance;
			break;
		}
		currentSegment = currentSegment->nextSegment;
	}
}

void setDirection(int newDirection){
	if(newDirection == 0){
		currentDirection = (currentDirection-1) % 4;	// 4 is the amount of possible directions
		if(currentDirection < 0){
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
	free(output);
}

int parseMessage(unsigned char message){
	int value = message;													// parsing the hex value to integer
	int line = value & (1<<7);
	int direction = value & (1<<6);
	int length = value & 63;

	if(direction > 0){
		direction = 1;
	}else{
		direction = 0;
	}
	createSegment(length, direction);
	sendMessage();
	return (line > 0 ? 1 : -1);									// returns, if the NIBO scanned a line

}

void sendMessage(){
	char answer = '1';
	write(serialDevice, &answer , 1);
}
