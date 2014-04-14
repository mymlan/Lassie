
#include <avr/io.h>
#include <avr/interrupt.h>
#include "../CommonLibrary/Common.h"
#include "Sensor_SPI.h"

static volatile uint8_t SPI_test = 0;

int main(void)
{
	SPI_sensor_init();
	sei();
	
	while(1)
	{
		if(SPI_should_give_sensor_data())
		{
			SPI_test = 0xff;
			SPI_slave_send_byte(0x03);
		}
	}
	return 0;
}

