// Standard Includes
#include <nibo/niboconfig.h>
#include <nibo/iodefs.h>
#include <nibo/bot.h>
// Display
#include <nibo/display.h>
// Grafikfunktionen
#include <nibo/gfx.h>
// Wartefunktionen
#include <nibo/delay.h>
// Coprozessorfunktionen
#include <nibo/copro.h>
// Kommunikationsfunktionen
#include <nibo/spi.h>
// Interrupts
#include <avr/interrupt.h>
// Ein- und Ausgabefunktionen
#include <stdio.h>

int currentState = 0;
int threshold_slopes = 90;
int threshold_front = 100;
int threshold_sides = 50;

enum STATES {
    NO_OBSTACLE,
    OBSTACLE_FRONT,
    OBSTACLE_LEFT,
    OBSTACLE_RIGHT,
    OBSTACLE_SIDES,
    OBSTACLE_DEADEND
};
void writeToDisplay(char *output) {
	gfx_fill(0);
	gfx_move(0, 0);
	gfx_print_text(output);
    return;
}
void turnLeft(){
	copro_setTargetRel(-27, 27, 10);
	delay(2000);
	copro_stop();
	return;
	/*
    copro_setSpeed(-10, 10);
    while(1 == 1){
    	copro_update();
    	if(copro_distance[2]/256 < 30){
    		copro_setSpeed(10, 10);
    		return;
    	}
    	delay(500);
    }
    */
}
void turnRight(){
	copro_setTargetRel(27, -27, 10);
	delay(2000);
	copro_stop();
	return;
    /*copro_setSpeed(10, -10);
    while(1 == 1){
    	copro_update();
    	if(copro_distance[2]/256 < 30){
    		copro_setSpeed(10, 10);
    		return;
    	}
    	delay(500);
    }
    */
}
void checkSides(){
    copro_update();
    if(copro_distance[0]/256 > copro_distance[4]/256){
        turnLeft();
    }else{
        turnRight();
    }
    return;
}
void driveBackwards(){
    while(1==1){
    	copro_setSpeed(-10, -10);
        copro_update();
        if(copro_distance[0]/256 < 20 && copro_distance[4]/256 < 20){
            delay(1000);
            turnRight();
            return;
        }else if(copro_distance[0]/256 < 25){
            delay(1000);
            turnRight();
            return;
        }else{
            delay(1000);
            turnLeft();
            return;
        }
        delay(1000);
    }
}

int main() {
	sei();
	bot_init();
	spi_init();

	display_init();
	gfx_init();

    copro_ir_startMeasure();
	// Endlosschleife
	while (1 == 1) {
        copro_update();
        //copro_setSpeed(10, 10);
        if(copro_distance[2]/256 >= threshold_front){
        	if ((copro_distance[0]/256 >= threshold_sides && copro_distance[4]/256 >= threshold_sides) || (copro_distance[1]/256 >= threshold_slopes && copro_distance[3]/256 >= threshold_slopes)){
        		currentState = OBSTACLE_DEADEND;
        	}else{
        		currentState = OBSTACLE_FRONT;
        	}
        }else if(copro_distance[1]/256 >= threshold_slopes && copro_distance[3]/256 < threshold_slopes){
        	currentState = OBSTACLE_RIGHT;
        }else if(copro_distance[3]/256 >= threshold_slopes && copro_distance[1]/256 < threshold_slopes){
            currentState = OBSTACLE_LEFT;
        }else if(copro_distance[3]/256 >= threshold_slopes && copro_distance[1]/256 >= threshold_slopes){
            currentState = OBSTACLE_SIDES;
        }else{
            currentState = NO_OBSTACLE;
        }

        switch (currentState){
            case NO_OBSTACLE:
                writeToDisplay("NO OBSTACLE");
                break;
            case OBSTACLE_FRONT:
            	copro_stop();
                writeToDisplay("OBSTACLE: FRONT");
                //checkSides();
                break;
            case OBSTACLE_LEFT:
            	copro_stop();
                writeToDisplay("OBSTACLE: LEFT");
                //turnRight();
                break;
            case OBSTACLE_RIGHT:
            	copro_stop();
                writeToDisplay("OBSTACLE: RIGHT");
                //turnLeft();
                break;
            case OBSTACLE_SIDES:
                writeToDisplay("OBSTACLE: TUNNEL");
                break;
            case OBSTACLE_DEADEND:
            	copro_stop();
                writeToDisplay("OBSTACLE: DEADEND");
                driveBackwards();
                break;
        }
		delay(1000);
	}
}
