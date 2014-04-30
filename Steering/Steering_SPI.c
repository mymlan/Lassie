
#define F_CPU (18432000L)

#include <avr/io.h>
#include <avr/interrupt.h>
#include "../CommonLibrary/Common.h"
#include "Steering_SPI.h"
#include "Steering_functions.h"
#include <util/delay.h>

static volatile uint8_t last_auto_decision;

static volatile uint8_t error;
static volatile uint8_t test;
static volatile uint8_t test2;
static volatile uint8_t test3;
static volatile uint8_t test4;
static volatile uint8_t sensor_data[7];

void SPI_steering_init(void)
{
	SPCR = 0xC0; //Aktiverar avbrott från SPI, aktiverar SPI, sätter modul till slave.
	DDRB = 0x40; //Sätter MISO till utgång
	
	DDRD = 0x30; //utgångar på PD4 och PD5
	PORTD = 0; //test
	PORTD = 0x20; //test
	PORTD = 0; //test
	
	last_auto_decision = NO_NEED_TO_REGULATE;

	error = 0;
	test = 0;
	test2 = 0;
	test3 = 0;
	test4 = 0;
}

static void SPI_steering_recieve_sensor_data(volatile uint8_t *sensor_data)
{
	uint8_t number_of_bytes_in_data = NUMBER_OF_BYTES_IR_SENSOR_DATA;
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
		case ID_BYTE_IR_SENSOR_DATA:
		{
			SPI_steering_recieve_sensor_data(sensor_data);
			switch(last_auto_decision)
			{
				case NO_NEED_TO_REGULATE:
				break;
				case REGULATED_FORWARD:
				Forward_regulated(sensor_data[5], sensor_data[6]);
				break;
				case REGULATED_BACKWARD:
				Backward_regulated(sensor_data[5], sensor_data[6]);
				break;
				default:
				error = 1;
				break;
			}
			
			break;
		}
		case ID_BYTE_MANUAL_DECISIONS:
		{
			test3 = 1;
			uint8_t manual_decision = SPI_steering_recieve_byte();
			test4 = manual_decision;
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
				error = 1;
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
				last_auto_decision = NO_NEED_TO_REGULATE;
				break;
				case COMMAND_FORWARD: Forward_regulated(0, 0);
				last_auto_decision = REGULATED_FORWARD;
				break;
				case COMMAND_BACKWARD: Backward_regulated(0, 0);
				last_auto_decision = REGULATED_BACKWARD;
				break;
				case COMMAND_ROTATE_RIGHT: Rotate_right();
				last_auto_decision = NO_NEED_TO_REGULATE;
				break;
				case COMMAND_ROTATE_LEFT: Rotate_left();
				last_auto_decision = NO_NEED_TO_REGULATE;
				break;
				case COMMAND_OPEN_CLAW: Open_claw();
				last_auto_decision = NO_NEED_TO_REGULATE;
				break;
				case COMMAND_CLOSE_CLAW: Close_claw();
				last_auto_decision = NO_NEED_TO_REGULATE;
				break;
				default: Stop();
				error = 1;
				last_auto_decision = NO_NEED_TO_REGULATE;
				break;
			}
			break;
		}
		default:
		error = 1;
		break;
	}
	sei();
}

