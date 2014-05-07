/*
 * Sensor.c
 *
 * Created: 2014-03-29 14:22:07
 *  Author: HuGu
 */ 


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
	
	//F�r att kunna f�rb�ttre regleringen finns f�ljande while
	/*while(1)
	{
		if(SPI_sensor_should_give_ir_sensor_data())
		{
			SPI_sensor_send_sensor_data(ir_sensor_data);
		}
	}*/
	
	 
	
    while(1)
	{
		if(SPI_sensor_should_give_distance())
		{
			uint8_t distance;
			uint8_t moved_distance = 0;
			moved_distance = ((reflex_count*16) / 10);
			distance = moved_distance;
			SPI_sensor_send_data_byte(ID_BYTE_DISTANCE, distance);
		}
		else if (SPI_sensor_should_give_ir_sensor_data())
		{
			SPI_sensor_send_sensor_data(ir_sensor_data);		
		}
		else if (SPI_sensor_should_start_angular_rate_sensor())
		{
			ACSR |= (1<<ACD);  //St�nger av Analog Comparator (reflexsensor)
			ADMUX = (1<<ADLAR)|(1<<REFS0)|(1<<MUX1);
			count = 5;
		} 
		else
		{}
	}
	
}
			
			