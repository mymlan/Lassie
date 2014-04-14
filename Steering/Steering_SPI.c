
#define F_CPU (16000000L)

#include <avr/io.h>
#include <avr/interrupt.h>
#include "../CommonLibrary/Common.h"
#include "Steering_SPI.h"
#include <util/delay.h>

static volatile uint8_t has_recieved_sensor_data;
static volatile uint8_t byte_from_SPI;

void SPI_steering_init()
{
	SPCR = 0xC0; //Aktiverar avbrott från SPI, aktiverar SPI, sätter modul till slave.
	DDRB = 0x40; //Sätter MISO till utgång
	
	DDRD = 0x30; //utgångar på PD4 och PD5
	PORTD = 0; //test
	PORTD = 0x20; //test
	PORTD = 0; //test
	
	has_recieved_sensor_data = 0;
	byte_from_SPI = 0;
}

//Avbrottsrutin SPI transmission complete
ISR(SPI_STC_vect)
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

uint8_t SPI_have_recieved_sensor_data(void)
{
	uint8_t result = has_recieved_sensor_data;
	has_recieved_sensor_data = 0;
	return result;
}