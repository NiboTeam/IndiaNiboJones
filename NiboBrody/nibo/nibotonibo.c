#include "nibotonibo.h"
#include "uart0.h"

void sendMessageToNibo(int signal){
	// signal indicates the start or end of the music
	// this message addresses the NIBO, so the indication bit has to be 0
	int byte = signal;
	unsigned char message = byte & 127;
	int i = 0;
	while(1){
		if(!uart0_txfull()){
			uart0_putchar(message);
		}
		delay(200);
		if (!uart0_rxempty() || i == 25){
			// wait for answer
			break;
		}
		i++;
	}
}
