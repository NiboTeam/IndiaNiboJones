/*
 * mandatoryProtocol.c
 *
 *  Created on: 06.11.2018
 *      Author: hendriktanke
 */

#include <nibo/niboconfig.h>
#include <nibo/iodefs.h>
#include <nibo/bot.h>
#include <nibo/display.h>
#include <nibo/gfx.h>
#include <nibo/delay.h>
#include <nibo/copro.h>
#include <nibo/spi.h>
#include <nibo/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "gfxOutput.h"
#include "protocols.h"

#define OBSTACLE_IN_RANGE 20
#define FAR_DISTANCE 50
#define MEDIUM_DISTANCE 100
#define NEAR_DISTANCE 150
#define CRITICAL_DISTANCE 200

int main() {

	bot_init();
	spi_init();

	display_init();
	gfx_init();

	copro_ir_startMeasure();

	printDebug("Started!");
	delay(500);
	cleanDebug("        ");

	while(1 == 1){
		copro_update();
		printInfo("     ");
		char output[20];
		int current_distance = copro_distance[2] / 256;
		sprintf(output, "%3i ", current_distance);
		printInfo(output);
		if(copro_distance[2] / 256 >= MEDIUM_DISTANCE){
			cleanDebug("          ");
			printDebug("CRITICAL!!");
		}else{
			cleanDebug("          ");
			printDebug("Chillin...");
		}
		delay(400);
	}
}
