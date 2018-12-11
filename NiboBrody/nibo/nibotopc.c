#include "nibotopc.h"
#include "uart0.h"

void sendSegment(int recognizedLine, int direction, int leftOdometry, int rightOdometry){
	int averageOdometry = ((leftOdometry + rightOdometry) / 27) / 2;
	// 1 * 2 ^ 7 addresses the PC
	int byte = 128 + (64 * recognizedLine) + (32 * direction) + averageOdometry;
	unsigned char message = byte & 255;
	unsigned char answer;
	int i = 0;
	while(1){
		if(!uart0_txfull()){
			uart0_putchar(message);
		}
		delay(200);
		// wait for answer
		if(!uart0_rxempty()){
			answer = uart0_getchar();
			int byte = answer;
			if((byte & 128) != 128){
				// wrong indicator
				continue;
			}
			break;
		}else if (i == 25){
			break;
		}
		i++;
	}
}
