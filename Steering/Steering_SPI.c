
#define F_CPU (16000000L)

#include <avr/io.h>
#include <avr/interrupt.h>
#include "../CommonLibrary/Common.h"
#include "Steering_SPI.h"
#include <util/delay.h>

static volatile uint8_t error;

void SPI_steering_init(void)
{
	SPCR = 0xC0; //Aktiverar avbrott fr�n SPI, aktiverar SPI, s�tter modul till slave.
	DDRB = 0x40; //S�tter MISO till utg�ng
	
	DDRD = 0x30; //utg�ngar p� PD4 och PD5
	PORTD = 0; //test
	PORTD = 0x20; //test
	PORTD = 0; //test

	error = 0;
	
}

static void SPI_steering_recieve_sensor_data(uint8_t *sensor_data)
{
	uint8_t number_of_bytes_in_data = NUMBER_OF_BYTES_SENSOR_DATA;
	while(number_of_bytes_in_data != 0)
	{
		if (SPSR & (1<<SPIF))
		{
			sensor_data[(number_of_bytes_in_data - 1)] = SPDR;
			number_of_bytes_in_data--;
		}
	}
}
static uint8_t SPI_steering_recieve_byte(void)
{
	uint8_t number_of_bytes_in_data = 1;
	while(number_of_bytes_in_data != 0)
	{
		if (SPSR & (1<<SPIF))
		{
			number_of_bytes_in_data--;
		}
	}
	return SPDR;
}

//Avbrottsrutin SPI transmission complete
ISR(SPI_STC_vect)
{
	cli();
	uint8_t byte_from_SPI = SPDR;
	switch (byte_from_SPI)
	{
		case ID_BYTE_SENSOR_DATA:
		{
			uint8_t sensor_data[6];
			SPI_steering_recieve_sensor_data(sensor_data);
			// g�r det som ska h�nda med sensor_data!
			break;
		}
		case ID_BYTE_GUIDED_DECISIONS:
		{
			uint8_t guided_decision = SPI_steering_recieve_byte();
			(void)guided_decision;
			break;
		}
		case ID_BYTE_AUTO_DECISIONS:
		{
			uint8_t auto_decision = SPI_steering_recieve_byte();
			(void)auto_decision;
			break;
		}
		default:
		error = 1;
		break;
	}
	sei();
}

