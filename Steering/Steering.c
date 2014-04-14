/*
* Steering.c
*
* Created: 3/31/2014 2:07:31 PM
* Author: henba136
*/
//#define F_CPU (16000000L)

#include <avr/io.h>
#include <avr/interrupt.h>
#include "../CommonLibrary/Common.h"
//#include <util/delay.h>

static volatile uint8_t has_recieved_sensor_data;
static volatile uint8_t byte_from_SPI;

static void SPI_init()
{
	DDRB = 0x40; //Sätter MISO till utgång
	SPCR = 0xC0; //Aktiverar avbrott från SPI, aktiverar SPI, sätter modul till slave.

	DDRD = 0x30; //utgångar på PD4 och PD5
	PORTD = 0; //test
	PORTD = 0x20; //test
	PORTD = 0; //test
	
	has_recieved_sensor_data = 0;
	byte_from_SPI = 0;
}
ISR(SPI_STC_vect) //Den avbrotsrutin som sensorn går in i då komm skickat data.
{
	byte_from_SPI = SPDR;
	switch (byte_from_SPI)
	{
		case ID_BYTE_SENSOR_DATA:
		has_recieved_sensor_data = 1;
		PORTD = 0;
		PORTD = 0x20;
		PORTD = 0;
		break;
		default:
		break;
	}
}


int main(void)
{
	SPI_init();
	sei();

	while (1)
	{		
		;
	}
	
	return 0;
}