
#include <avr/io.h>
#include <avr/interrupt.h>
#include "Communication_init.h"
#include "../CommonLibrary/Common.h"

void init_button_search ()
{
	DDRC &= ~(1 << DDC0);
	PORTC |= (1 << PORTC0);
	
	PCICR |= (1 << PCIE2);
	PCMSK2 |= (1 << PCINT16);
}

ISR(PCINT2_vect)
{
	//level++;
	PCICR |= (0 << PCIE2);
	PCMSK2 |= (0 << PCINT16);
}


void init_button_deliver()
{
	DDRD &= ~(1 << DDD4);
	PORTD |= (1 << PORTD4);
	
	PCICR |= (1 << PCIE3);
	PCMSK3 |= (1 << PCINT28);
}

ISR(PCINT3_vect)
{
	//if(level == 4)
	//{
	//level++;
	PCICR |= (0 << PCIE3);
	PCMSK3 |= (0 << PCINT28);
	//}
}