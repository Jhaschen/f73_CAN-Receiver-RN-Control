#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "f73-rncontrol-lib/uart.h"
#include "f73-rncontrol-lib/adc.h"
#include "f73-rncontrol-lib/button.h"
#include "f73-rncontrol-lib/led.h"
#include "f73-rncontrol-lib/counter.h"

#include "can.h"

volatile uint32_t timeStep;



int main() {
    can_t recvMsg;

    uint32_t currentTimeStep = 0;
    uint32_t lastToggle = 0;
    uint32_t lastStat = 0;
    
    
    ledInit();
    uartInit(57600, 8, 'N', 1);
    can_init(BITRATE_500_KBPS); // CAN init 500 kbit/s

    // Counter 1 CTC Mode 1ms
    counter0SetCompare(249);
    counter0EnableCompareMatchInterrupt();
    counter0EnableCTC; // set CTC mode
    counter0Start(ATMEGA32_COUNTER_0_PRESCALER_64);

    sei();

  

    for (;;)
	{
        cli();
        currentTimeStep = timeStep; // get copy in atomic operation (not interrupted by ISR)
        sei();

        
        int status = can_get_message(&recvMsg);
        if (status) { // did we receive something?
            printf("%8lu ms RecCanMsg: ID 0x%02x; Length %2d; Data: ", currentTimeStep, recvMsg.id, recvMsg.length);
                for (uint8_t i = 0; i < recvMsg.length; i++)
                {
                    printf("0x%02x ", recvMsg.data[i]);
                }

                printf("\r\n");
        }    

      
	}
    return 0;   
}

ISR(TIMER0_COMP_vect) {
    timeStep++;
}