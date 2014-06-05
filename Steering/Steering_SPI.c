/*
 * Steering_SPI.c 
 * F�rfattare: mikiv293, linka950
 * 2014-06-05
 * version 1.0
 */

#define F_CPU (18432000L)

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "../CommonLibrary/Common.h"
#include "Steering_SPI.h"
#include "Steering_functions.h"

//-------------VARIABLER/KONSTANTER---------------//
static volatile uint8_t last_auto_decision;

//-----------------STATIC FUNKTIONER----------------//
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

//----------------INITIERINGSFUNKTION----------------//
void SPI_steering_init(void)
{
	SPCR = 0xC0; //Aktiverar avbrott fr�n SPI, aktiverar SPI, s�tter modul till slave.
	DDRB = 0x40; //S�tter MISO till utg�ng
	
	last_auto_decision = NO_NEED_TO_REGULATE;
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
					PORTA = 0xEC; //Sp�rutskrift p� 7-segmetdisplay
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
					PORTA = 0x90; //Sp�rutskrift p� 7-segmetdisplay
					break;
				case COMMAND_ROTATE_LEFT: Rotate_left();
					PORTA = 0x91; //Sp�rutskrift p� 7-segmetdisplay
					break;
				case COMMAND_TURN_RIGHT: Turn_right();
					PORTA = 0x92; //Sp�rutskrift p� 7-segmetdisplay
					break;
				case COMMAND_TURN_LEFT: Turn_left();
					PORTA = 0x93; //Sp�rutskrift p� 7-segmetdisplay
					break;
				case COMMAND_OPEN_CLAW: Open_claw();
					break;
				case COMMAND_CLOSE_CLAW: Close_claw();
					break;
				default: Stop();
					PORTA = 0xEF; //Sp�rutskrift p� 7-segmetdisplay
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
				case COMMAND_FORWARD: Forward_regulated(90,100);
					PORTA = 0x30; //Sp�rutskrift p� 7-segmetdisplay
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
					PORTA = 0x40; //Sp�rutskrift p� 7-segmetdisplay
					last_auto_decision = NO_NEED_TO_REGULATE;
					break;
				case COMMAND_ROTATE_LEFT: Rotate_left();
					PORTA = 0x41; //Sp�rutskrift p� 7-segmetdisplay
					last_auto_decision = NO_NEED_TO_REGULATE;
					break;
				case COMMAND_TIGHT_TURN_RIGHT: Tight_turn_right();
					PORTA = 0x70; //Sp�rutskrift p� 7-segmetdisplay
					last_auto_decision = NO_NEED_TO_REGULATE;
					break;
				case COMMAND_TIGHT_TURN_LEFT: Tight_turn_left();
					PORTA = 0x71; //Sp�rutskrift p� 7-segmetdisplay
					last_auto_decision = NO_NEED_TO_REGULATE;
					break;
				case COMMAND_OPEN_CLAW: Open_claw();
					last_auto_decision = NO_NEED_TO_REGULATE;
					break;
				case COMMAND_CLOSE_CLAW: Close_claw();
					last_auto_decision = NO_NEED_TO_REGULATE;
					break;
				case COMMAND_STOP_1: Stop();
					//PORTA = 0x21; //Sp�rutskrift p� 7-segmetdisplay
					last_auto_decision = NO_NEED_TO_REGULATE;
					break;
				case COMMAND_STOP_2: Stop();
					//PORTA = 0x22; //Sp�rutskrift p� 7-segmetdisplay
					last_auto_decision = NO_NEED_TO_REGULATE;
					break;
				case COMMAND_STOP_3: Stop();
					//PORTA = 0x23; //Sp�rutskrift p� 7-segmetdisplay
					last_auto_decision = NO_NEED_TO_REGULATE;
					break;
				case COMMAND_STOP_4: Stop();
					//PORTA = 0x24; //Sp�rutskrift p� 7-segmetdisplay
					last_auto_decision = NO_NEED_TO_REGULATE;
					break;								
				case COMMAND_STOP_5: Stop();
					//PORTA = 0x25; //Sp�rutskrift p� 7-segmetdisplay
					last_auto_decision = NO_NEED_TO_REGULATE;
					break;
				case COMMAND_STOP_6: Stop();
					//PORTA = 0x26; //Sp�rutskrift p� 7-segmetdisplay
					last_auto_decision = NO_NEED_TO_REGULATE;
					break;
				default: Stop();
					//PORTA = 0xEA; //Sp�rutskrift p� 7-segmetdisplay
					last_auto_decision = NO_NEED_TO_REGULATE;
					break;
			}
			break;
		}
		default:
			PORTA = 0xEE; //Sp�rutskrift p� 7-segmetdisplay
			break;
	}
}

