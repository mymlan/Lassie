
#define F_CPU (18432000L)

#include <avr/io.h>
#include <avr/interrupt.h>
#include "../CommonLibrary/Common.h"
#include "Steering_SPI.h"
#include "Steering_functions.h"
#include <util/delay.h>

static volatile uint8_t error;
static volatile uint8_t test;
static volatile uint8_t test2;
static volatile uint8_t test3;

// uint8_t regulated_order;

void SPI_steering_init(void)
{
	SPCR = 0xC0; //Aktiverar avbrott fr�n SPI, aktiverar SPI, s�tter modul till slave.
	DDRB = 0x40; //S�tter MISO till utg�ng
	
	DDRD = 0x30; //utg�ngar p� PD4 och PD5
	PORTD = 0; //test
	PORTD = 0x20; //test
	PORTD = 0; //test

	error = 0;
	test = 0;
	test2 = 0;
	test3 = 0;
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
	test = 1;
	cli();
	uint8_t byte_from_SPI = SPDR;
	test2 = byte_from_SPI;
	switch (byte_from_SPI)
	{
		test3 = 1;
		case ID_BYTE_SENSOR_DATA:
		{
			uint8_t sensor_data[6];
			SPI_steering_recieve_sensor_data(sensor_data);
			// g�r det som ska h�nda med sensor_data!
			/*
			switch(regulated_order) // =0 innbe�r ingen reglering, =1 reglering fram, =2 reglering bak
			{
				case 1: Forward_regulated(sensor_data[5], sensor_data[6]);
				break;
				case 2: Backward_regulated();
				break;
				case 0:
				break;
			*/
			break;
		}
		case ID_BYTE_MANUAL_DECISIONS:
		{

			uint8_t manual_decision = SPI_steering_recieve_byte();
			switch(manual_decision)
			{
				case COMMAND_STOP: Stop();
				break;
				case COMMAND_FORWARD: Forward();
				break;
				case COMMAND_BACKWARD: Backward();
				break;
				case COMMAND_ROTATE_RIGHT: Rotate_right();
				break;
				case COMMAND_ROTATE_LEFT: Rotate_left();
				break;
				case COMMAND_TURN_RIGHT: Turn_right();
				break;
				case COMMAND_TURN_LEFT: Turn_left();
				break;
				case COMMAND_OPEN_CLAW: Open_claw();
				break;
				case COMMAND_CLOSE_CLAW: Close_claw();
				break;
				default: Stop();
				break;
			}
			break;
		}
		case ID_BYTE_AUTO_DECISIONS:
		{
			uint8_t auto_decision = SPI_steering_recieve_byte();
			switch(auto_decision)
			{
				case COMMAND_STOP: Stop();
				//regulated_order = 0;
				break;
				case COMMAND_FORWARD: Forward_regulated(0, 0);
				//regulated_order = 1;
				break;
				case COMMAND_BACKWARD: Backward_regulated();
				//regulated_order = 2;
				break;
				case COMMAND_ROTATE_RIGHT: Rotate_right();
				//regulated_order = 0;
				break;
				case COMMAND_ROTATE_LEFT: Rotate_left();
				//regulated_order = 0;
				break;
				case COMMAND_OPEN_CLAW: Open_claw();
				//regulated_order = 0;
				break;
				case COMMAND_CLOSE_CLAW: Close_claw();
				//regulated_order = 0;
				break;
				default: Stop();
				//regulated_order = 0;
				break;
			}
			(void)auto_decision;
			break;
		}
		default:
		error = 1;
		break;
	}
	sei();
}

