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
	test = 0;
	
	SPI_steering_init();
	sei();
	while(1)
	{}
	return 0;
}