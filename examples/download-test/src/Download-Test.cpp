/*
===============================================================================
 Name        : main.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

#ifdef __USE_CMSIS
#include "LPC11Uxx.h"
#endif

#include <sblib/digital_pin.h>
#include <sblib/io_pin_names.h>
#include <sblib/timeout.h>
#include <sblib/interrupt.h>
#include <sblib/serial.h>

Timeout blinky;

int main(void)
{
	pinMode(PIN_INFO, OUTPUT);
	pinMode(PIN_RUN,  OUTPUT);
	digitalWrite(PIN_RUN, 1);
    SysTick_Config(SystemCoreClock / 1000);
    interrupts();
	blinky.start (1000);
	serial.begin(115200);
	serial.println("Hello World via KNX download!");
    // TODO: insert code here

    // Force the counter to be placed into memory
    volatile static int i = 0 ;
    // Enter an infinite loop, just incrementing a counter
    while(1) {
        i++ ;
        digitalWrite(PIN_INFO, i & 0x00080000);
        if (blinky.expired())
        {
        	blinky.start (1000);
            digitalWrite(PIN_RUN, !digitalRead(PIN_RUN));
        }
    }
    return 0 ;
}
