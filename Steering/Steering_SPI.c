
#define F_CPU (18432000L)

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "../CommonLibrary/Common.h"
#include "Steering_SPI.h"
#include "Steering_functions.h"

static volatile uint8_t last_auto_decision;

static volatile uint8_t error;

void SPI_steering_init(void)
{
	SPCR = 0xC0; //Aktiverar avbrott från SPI, aktiverar SPI, sätter modul till slave.
	DDRB = 0x40; //Sätter MISO till utgång
	
	last_auto_decision = NO_NEED_TO_REGULATE;

	error = 0;
}

static void SPI_steering_recieve_sensor_data(uint8_t *sensor_data)
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
	uint8_t byte_from_SPI = SPDR;
	switch (byte_from_SPI)
	{
		case ID_BYTE_IR_SENSOR_DATA:
		{
			uint8_t sensor_data[7];
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
					//PORTA = 0x20; // Error
					break;
				case COMMAND_FORWARD: Forward_regulated(90, 100);
					last_auto_decision = REGULATED_FORWARD;
					break;
				case COMMAND_BACKWARD: Backward_regulated(90, 100);
					last_auto_decision = REGULATED_BACKWARD;
					break;
				case COMMAND_FORWARD_NOT_REGULATED: Forward_regulated(90, 100);
					last_auto_decision = NO_NEED_TO_REGULATE;
					break;
				case COMMAND_BACKWARD_NOT_REGULATED: Backward_regulated(90, 100);
					last_auto_decision = NO_NEED_TO_REGULATE;
					break;
				case COMMAND_ROTATE_RIGHT: Rotate_right();
					last_auto_decision = NO_NEED_TO_REGULATE;
					break;
				case COMMAND_ROTATE_LEFT: Rotate_left();
					last_auto_decision = NO_NEED_TO_REGULATE;
					break;
				case COMMAND_TIGHT_TURN_RIGHT: Tight_turn_right();
					last_auto_decision = NO_NEED_TO_REGULATE;
					break;
				case COMMAND_TIGHT_TURN_LEFT: Tight_turn_left();
					last_auto_decision = NO_NEED_TO_REGULATE;
					break;
				case COMMAND_OPEN_CLAW: Open_claw();
					last_auto_decision = NO_NEED_TO_REGULATE;
					break;
				case COMMAND_CLOSE_CLAW: Close_claw();
					last_auto_decision = NO_NEED_TO_REGULATE;
					break;
				case COMMAND_STOP_1: Stop();
					//PORTA = 0x21; // Error
					last_auto_decision = NO_NEED_TO_REGULATE;
					break;
				case COMMAND_STOP_2: Stop();
					//PORTA = 0x22; // Error
					last_auto_decision = NO_NEED_TO_REGULATE;
					break;
				case COMMAND_STOP_3: Stop();
					//PORTA = 0x23; // Error
					last_auto_decision = NO_NEED_TO_REGULATE;
					break;
				case COMMAND_STOP_4: Stop();
					//PORTA = 0x24; // Error
					last_auto_decision = NO_NEED_TO_REGULATE;
					break;								
				case COMMAND_STOP_5: Stop();
					//PORTA = 0x25; // Error
					last_auto_decision = NO_NEED_TO_REGULATE;
					break;
				case COMMAND_STOP_6: Stop();
					//PORTA = 0x26; // Error
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
			PORTA = 0xEE;
			error = 1;
			break;
	}
}

