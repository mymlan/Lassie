
#define F_CPU (16000000L) //definerar klockfrekvens för att _delay_us ska funka

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "../CommonLibrary/Common.h"
#include "Communication_SPI.h"

int main(void)
{
	COMMON_SET_PIN(PORTA, PORTA7);

	SPI_Master_init();
	sei();
	uint8_t test_sensor_data[6];
	
	test_sensor_data[0] = 0;//plockar ut värdet, alt stoppar in. 0-5
	test_sensor_data[1] = 1;
	test_sensor_data[2] = 2;
	test_sensor_data[3] = 3;
	test_sensor_data[4] = 4;
	test_sensor_data[5] = 5;
	
	SPI_Master_send_to_steering(0x01, test_sensor_data);
    while(1)
    {
        ;
    }
	return 0;
}

