
#include <avr/io.h>
#include <avr/interrupt.h>
#include "Communication_init.h"
#include "../CommonLibrary/Common.h"
#include "Communication_map.h"
#include "Communication_SPI.h"

void Init_button_search()
{
	COMMON_CLEAR_PIN(DDRC, DDC0);
	COMMON_SET_PIN(PORTC, PORTC0);
	COMMON_SET_BIT(PCICR, PCIE2);
	COMMON_SET_BIT(PCMSK2, PCINT16);
}

ISR(PCINT2_vect)
{
	level++;
	SPI_map_send_command_to_steering(ID_BYTE_AUTO_DECISIONS, COMMAND_FORWARD);
	COMMON_CLEAR_BIT(PCICR, PCIE2); //Stänger av avbrottet
	COMMON_CLEAR_BIT(PCMSK2, PCINT16); //Stänger av avbrottet
	COMMON_TOGGLE_PIN(PORTA, PORTA1);
}


void Init_button_deliver()
{
	COMMON_CLEAR_PIN(DDRD, DDD4);
	COMMON_SET_PIN(PORTD, PORTD4);
	COMMON_SET_BIT(PCICR, PCIE3);
	COMMON_SET_BIT(PCMSK3, PCINT28);
}

ISR(PCINT3_vect)
{
	if(level == WAIT_FOR_ITEM)
	{
		level = GRAB_ITEM_AND_CALC_DIJK;
		COMMON_CLEAR_BIT(PCICR, PCIE3); //Stänger av avbrottet
		COMMON_CLEAR_BIT(PCMSK3, PCINT28); //Stänger av avbrottet
	}
}

void Timer_init()
{
	//sätter timer
	TCCR1A = 0x00;
	TCCR1B = 0x0B;
	TCCR1C = 0x00;

	COMMON_SET_BIT(TIMSK1, OCIE1A); // Enable interrupt	
	
	OCR1A = TIMER_COMPARE_NUMBER;
}

ISR(TIMER1_COMPA_vect)
{
	SPI_Master_send_id_byte_to_sensor(ID_BYTE_GIVE_IR_SENSOR_DATA);
}