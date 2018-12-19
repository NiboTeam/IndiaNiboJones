/**
 * @file main.c
 * @author H.Tanke
 * @version 1.0
 * @brief This is the main file of program NIBOJones. In this file is the main located.
 * Its the start point of program NIBOJones
 */

#include <nibo/niboconfig.h>
#include <nibo/iodefs.h>
#include <nibo/bot.h>
#include <nibo/display.h>
#include <nibo/floor.h>
#include <nibo/gfx.h>
#include <nibo/delay.h>
#include <nibo/copro.h>
#include <nibo/spi.h>
#include <nibo/leds.h>
#include <nibo/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>

#include "n2switchS3.h"
#include "protocols.h"
#include "gfxOutput.h"
#include "uart0.h"

int coproTicksLeft;
int coproTicksRight;

/**
 * The XBee communication has to be initialized before use.
 */
void initUART0(){
	uart0_set_baudrate(9600);
	uart0_enable();
}

/**
 * This will safe data of the co-processor for sending to NIBOBrody.
 * Those data are ticks of both wheels moved since last reset
 */
void gatherCoproData(){
	coproTicksLeft = copro_ticks_l;
	coproTicksRight = copro_ticks_r;
}

/**
 * Sends a single part of the measured part of structure to NIBOBrody.
 * @param recognizedLine set this to 1 if a black line was crossed by measuring last part
 * @param direction direction in which the NIBO turned before measuring this part.
 * @param leftOdometry ticks of the left wheel gathered since last reset
 * @param rightOdometry ticks of the left wheel gathered since last reset
 * @return
 */
int sendSegment(int recognizedLine, int direction, int leftOdometry, int rightOdometry){
	int averageOdometry = ((leftOdometry + rightOdometry) / 27) / 2;
	// 128 is address of NIBOBrody, function to minimize measured length to a more appropriate length for sending.
	int byte = 128 + (64 * recognizedLine) + (32 * direction) + averageOdometry;
	unsigned char message = byte & 255;
	int finishedSuccessfull = 0;
	if(!uart0_txfull()){
		uart0_putchar(message);
		delay(200);
		finishedSuccessfull = 1;
	}
	return finishedSuccessfull;
}

/**
 * This method capsulates gathering co-processor data and sending to NIBOBrody by using XBee communication.
 * @return if sending was successful this returns 1.
 */
int sendData(){
	int successful = 0;
	gatherCoproData();
	successful = sendSegment(getRecognizedBlackLine(), getTurnDirection(), coproTicksLeft, coproTicksRight);
	copro_resetOdometry(0, 0);

	return successful;
}

/**
 * This is the main part of the program NIBOJones. It contains the state machine and controll simple mechanics of the NIBO
 * like switching on LEDs.
 * @return
 */
int main() {
	machine_State = WAITING;
	measure_State = FINISHED;
	run_direction = UNKNOWN;
	int isInitialized = 0;
	int last_machineState = FINISHING_RUN;

	initUART0();
	printDebug("Init Xbee...");
	delay(5000);
	cleanDebug(15);

	sei();
	bot_init();
	spi_init();
	leds_init();

	display_init();
	gfx_init();

	copro_ir_startMeasure();
	floor_init();
	int sendingResult;
	// machine state holder
	while (1 == 1) {
		switch (machine_State) {
		case INITIALIZATION:
			init_protocol();
			machine_State = RUNNING_FORWARD;
			break;
		case RUNNING_FORWARD:
			runForward_protocol();
			break;
		case TRACK_CORRECTION:
			trackCorrection_protocol();
			break;
		case TURNING_INSIDE:
			leds_set_status(LEDS_OFF, 4);
			leds_set_status(LEDS_OFF, 5);
			turnInside_protocol();
			if(measure_State == STARTED){
				machine_State = SENDING_DATA;
			}else{
				machine_State = RUNNING_FORWARD;
			}
			break;
		case TURNING_OUTSIDE:
			leds_set_status(LEDS_OFF, 4);
			leds_set_status(LEDS_OFF, 5);
			turnOutside_protocol();
			machine_State = RUNNING_FORWARD;
			copro_setSpeed(10, 10);
			delay(2000);
			if(measure_State == STARTED){
				machine_State = SENDING_DATA;
			}else{
				machine_State = RUNNING_FORWARD;
			}
			break;
		case SENDING_DATA:
			sendingResult = sendData();
			if(sendingResult == 1){
				if(measure_State == FINISHED){
					machine_State = FINISHING_RUN;
				}else if(sendingResult == 1 && measure_State == STARTED){
					machine_State = RUNNING_FORWARD;
				}
			} else {
				machine_State = WAITING;
				measure_State = FINISHED;
				printDebug("Connection failed.");
				delay(1000);
				cleanDebug(20);
				printDebug("Aborting..");
				delay(100);
			}
			setRecognizedBlackLine(0);
			break;
		case FINISHING_RUN:
			copro_stop();
			finishRun_protocol(); //Umriss anzeigen
			printDebug("Press s3 for continue.");
			while(1){
				if (s3_was_pressed()){
					break;
				}
			}
			machine_State = WAITING;
			break;
		case WAITING:
			copro_stop();
			if (s3_was_pressed()) {
				if(isInitialized == 1){
					machine_State = RUNNING_FORWARD;
				}else{
					machine_State = INITIALIZATION;
					isInitialized = 1;
				}
			}
			break;
		}

		if (last_machineState != machine_State) {
			printMachineState(machine_State);
		}

		last_machineState = machine_State;
	}
}

