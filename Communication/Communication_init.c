/*
 * CFile1.c
 *
 * Created: 2014-05-14 08:31:42
 *  Author: Hugo
 */ 
#include <avr/io.h>
#include <avr/interrupt.h>

void init_button ()
{
	DDRC &= ~(1 << DDC0);
	PORTC |= (1 << PORTC0);
	
	PCICR |= (1 << PCIE2);
	PCMSK2 |= (1 << PCINT16);
}