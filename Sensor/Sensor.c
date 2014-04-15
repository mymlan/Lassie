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


int main(void)

{
	SPI_sensor_init();
	sei();
	
	
	uint8_t sensor_data[6];//pekaren heter sensor_data
	sensor_data[1] = 3;//plockar ut värdet, alt stoppar in. 0-5
	//behöver inte vara volatile, loopar aldrig över den
	
	SPI_slave_send_byte(sensor_data[1]);
	
	while(1)
	{
	}
}

