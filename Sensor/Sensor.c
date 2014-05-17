#include <avr/io.h>
#include <avr/interrupt.h>
#include "../CommonLibrary/Common.h"

#include "Sensor_SPI.h"
#include "Sensor_cm_converter.h"

int main(void)
{
	USART_init();
  	init_interrupts();
	SPI_sensor_init();
	sei();
	
    while(1)
	{
		/*if(MANUAL_DECISIONS_ACTIVATED)
		{
			init_button_calibrate_angular_sensor();
			while(1)
			{
				
			}
		}else 
		{
			;
		}*/
	}
	
}
			
			