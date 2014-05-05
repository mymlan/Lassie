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

static volatile uint8_t test1;
uint8_t test_ir_sensor_data[7];

int main(void)
{
	USART_init();
  	init_interrupts();
	SPI_sensor_init();
	sei();
	
	test_ir_sensor_data[0] = 10;
	test_ir_sensor_data[1] = 11;
	test_ir_sensor_data[2] = 12;
	test_ir_sensor_data[3] = 13;
	test_ir_sensor_data[4] = 14;
	test_ir_sensor_data[5] = 3;
	test_ir_sensor_data[6] = 5;
	
	while(1)
	{
		/*if(SPI_sensor_should_give_ir_sensor_data())
		{
			test1 = 1;
			SPI_sensor_send_sensor_data(ir_sensor_data);
		}*/
	}
	
	 
	/*
    while(1)
	{
		if(SPI_sensor_should_give_distance()) //skriv klart denna
		{
			//Skicka distans
			uint8_t distance[1];
			uint8_t moved_distance = 0;
			moved_distance = ((reflex_count*13) / 10);
			distance[0] = moved_distance;
			SPI_sensor_send(ID_BYTE_DISTANCE, distance);
		}
		else if (SPI_sensor_should_give_ir_sensor_data())
		{
			SPI_sensor_send(ID_BYTE_GIVE_IR_SENSOR_DATA, ir_sensor_data);		
		}
		else if (SPI_sensor_should_start_angular_rate_sensor())
		{
			ADMUX = (1<<ADLAR)|(1<<REFS0)|(1<<MUX1);
			count = 5;
		} 
		else
		{}
	}*/
	
	/*if {start_angle}
		{
			send_dist (reflex_count);
			ADMUX = (1<<ADLAR)|(1<<REFS0)|(1<<MUX1);
			count=5;
			
		} elseif {dist_req}
			{
				send_dist (reflex_count);
				reflex_count = 0;
			} 
			elseif {ir_req}
				{
			send_IR (vec(s1,s2,s3,s4,s5))
			send_angle (angle)
		         }
	
	USART_check //function Alt. av man aktiverar RFID här också
	ser ut som: Läs parallel in från usart och kolla om rätt ID
	om rätt, sätt en pinne kopplad till KOM avbrott
	*/
	return 0;
}
			
			