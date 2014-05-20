#include <avr/io.h>
#include <avr/interrupt.h>
#include "../CommonLibrary/Common.h"

#include "Sensor_SPI.h"
#include "Sensor_cm_converter.h"

int main(void)
{
	Sensor_init_AD_converter();
	Sensor_init_analog_comparator();
	Sensor_init_USART_for_RFID();	
	Sensor_init_button_for_calibrating_angular_rate_sensor();

  	SPI_sensor_init();
	Calibrate_angular_rate_sensor();
		
	sei();
	
    while(1)
	{
		/*if(SPI_sensor_should_handle_distance_to_RFID())
		{
			number_of_reflex_counts_to_RFID_requested = 1;
			reflex_count = 0;
		}*/
	}
	
}
			
			