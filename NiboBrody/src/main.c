#include <stdio.h>
#include <stdlib.h>

typedef enum{							// contains the possible directions the Nibo can take (top down perspective)
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

segment *head = NULL;					// points to the starting segment
char *output;							// will later on be filled with the outline

int columns = 0;						// Defines the size of the array
int rows = 0;

int startColumn = 0;					// Defines the starting point of the outline within the array
int startRow = 0;

void createSegment(int distance, DIRECTIONS direction);	// Appends a new segment to the outline
void parseArraySize();					// Parses the size of 'output'
void fillArray();
void printOutline();

int main(){
	setvbuf(stdout, NULL, _IONBF, 0);
	/*
	createSegment(3, 1);
	createSegment(1, 2);
	createSegment(2, 3);
	createSegment(1, 2);
	createSegment(3, 1);
	createSegment(1, 2);
	createSegment(3, 3);
	createSegment(1, 2);
	createSegment(3, 3);
	createSegment(5, 0);
	createSegment(2, 1);
	createSegment(1, 2);
	*/
	// /*
	createSegment(1,1);
	createSegment(1,2);
	createSegment(1,1);
	createSegment(1,2);
	createSegment(1,1);
	createSegment(2,0);
	createSegment(1,1);
	createSegment(3,2);
	createSegment(1,3);
	createSegment(1,2);
	createSegment(1,3);
	createSegment(1,0);
	createSegment(1,3);
	createSegment(1,0);
	createSegment(1,3);
	createSegment(2,0);
	// */
	parseArraySize();
	fillArray();
	printOutline();
	free(output);
}

void createSegment(int distance, DIRECTIONS direction){
	distance = distance * 3;			// Besser für den Nutzer
	if (head == NULL){
		head = malloc(sizeof(segment));
		head->distance = distance;
		head->direction = direction;
		head->nextSegment = NULL;
	}else{
		segment *newPart = malloc(sizeof(segment));
		newPart->direction = direction;
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

void printOutline(){
	for(int i = 0; i < rows; i++){
		for(int j = 0; j < columns; j++){
			printf("%c", output[j+(i*columns)]);
		}
		printf("\n");
	}
}
