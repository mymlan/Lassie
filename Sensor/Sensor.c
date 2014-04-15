/*
 * Sensor.c
 *
 * Created: 2014-03-29 14:22:07
 *  Author: HuGu
 */ 


#include <avr/io.h>
#include <avr/interrupt.h>
#include "Sensor_SPI.h"

uint8_t sensor1, sensor2, sensor3, sensor4, sensor5;

volatile uint8_t sensor_data[6];//pekaren heter sensor_data

static volatile uint8_t test;

int main(void)

{
	SPI_sensor_init();
	sei();
	
	

	sensor_data[0] = 6;//plockar ut värdet, alt stoppar in. 0-5
	sensor_data[1] = 7;
	sensor_data[2] = 8;
	sensor_data[3] = 9;
	sensor_data[4] = 10;
	sensor_data[5] = 11;
	//behöver inte vara volatile, loopar aldrig över den
	
	//SPI_slave_send_byte(sensor_data[1]);
	
	while(1)
	{
		if(SPI_should_give_sensor_data())
		{
			test = 0xff;
			//SPI_sensor_send(0x01, sensor_data);
		}
	}
	return 0;
}

