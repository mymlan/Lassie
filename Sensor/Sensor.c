#include <avr/io.h>
#include <avr/interrupt.h>
#include "../CommonLibrary/Common.h"

#include "Sensor_SPI.h"
#include "Sensor_cm_converter.h"

int main(void)
{
	init_variable();
	init_button_calibrate_angular_sensor();
	USART_init();
  	init_interrupts();
	SPI_sensor_init();
	calibrate_angular_rate_sensor();
		
	sei();
	
    while(1)
	{
		
	}
	
}
			
			