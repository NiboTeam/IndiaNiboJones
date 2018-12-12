#include "nibotonibo.h"
#include "uart0.h"

void sendMessageToNibo(int signal){
	// signal indicates the start or end of the music
	// this message addresses the NIBO, so the indication bit has to be 0
	int byte = signal;
	unsigned char message = byte & 255;
	unsigned char answer;
	int i = 0;
	while(1){
		if(!uart0_txfull()){
			uart0_putchar(message);
		}
		delay(200);
		if (!uart0_rxempty()){
			// wait for answer
			answer = uart0_getchar();
			byte = answer;
			if((byte & 128) != 0){
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
