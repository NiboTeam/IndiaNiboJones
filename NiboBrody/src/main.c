#include <stdio.h>
#include <stdlib.h>

typedef enum{
    NORTH = 0,
    EAST = 1,
    SOUTH = 2,
    WEST = 3
} DIRECTIONS;

typedef struct segment{
	float distance;
	DIRECTIONS direction;
	struct segment *nextSegment;
}segment;

// core element of the outline
segment *head = NULL;

// Appends a new segment to the outline
void createSegment(float distance, DIRECTIONS direction);
void getSegments();

int main(){
	setvbuf(stdout, NULL, _IONBF, 0);
	createSegment(6, 0);
	createSegment(4, 0);
	createSegment(7, 0);
	getSegments();
}

void createSegment(float distance, DIRECTIONS direction){
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
void getSegments(){
	segment *currentSegment = head;
	while(currentSegment != NULL){
		printf("%f %d\n", currentSegment->distance, currentSegment->direction);
		currentSegment = currentSegment->nextSegment;
	}
}
