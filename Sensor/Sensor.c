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


void init_ports(){
	DDRA = 0xFF;
	PORTB = 0xff;
}


int main(void)

{
	SPI_sensor_init();
	init_ports();
	ADCSRA |= 1<<ADPS2;
	ADMUX |= 1<<REFS0 | 1<<REFS1 | 1<<ADLAR | 2;
	ADCSRA |= 1<<ADIE;
	ADCSRA |= 1<<ADEN;
	
	sei();
	
	ADCSRA | 1<<ADSC;
	
	
	while(1)
	{
	}
}

ISR(ADC_vect)
{
	
	
	switch(ADMUX)
	{
		case 0xE0:
		sensor1 = ADCH;
		ADMUX = 0xE1;
		break;
		case 0xE1:
		sensor2 = ADCH;
		ADMUX = 0xE2;
		break;
		case 0xE2:
		sensor3 = ADCH;
		ADMUX = 0xE3;
		break;
		case 0xE3:
		sensor4 = ADCH;
		ADMUX = 0xE4;
		break;
		case 0xE4:
		sensor5 = ADCH;
		ADMUX = 0xE0;
		break;
		
		
	}
	
	
	
	ADCSRA | 1<<ADSC;
	
	
}
