/*
* Steering.c
*
* Created: 3/31/2014 2:07:31 PM
* Author: henba136
*/
#define F_CPU (16000000L)

#include <avr/io.h>
#include <avr/interrupt.h>
#include "Steering_SPI.h"
#include <util/delay.h>

static volatile uint8_t test;

int main(void)
{
	/*
	uint8_t sensor_data[6];//pekaren heter sensor_data
	sensor_data[1] = 3;//plockar ut v�rdet, alt stoppar in. 0-5
	//beh�ver inte vara volatile, loopar aldrig �ver den*/
	test = 0;
	
	while(1)
	{
		if(SPI_have_recieved_sensor_data())
		{
			test = 0xff;
		}
	}
	return 0;
}