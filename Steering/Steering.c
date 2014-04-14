/*
* Steering.c
*
* Created: 3/31/2014 2:07:31 PM
* Author: henba136
*/
//#define F_CPU (16000000L)

#include <avr/io.h>
#include <avr/interrupt.h>
//#include <util/delay.h>

static void SPI_init()
{
	DDRB = 0x40; //S�tter MISO till utg�ng
	SPCR = 0xC0; //Aktiverar avbrott fr�n SPI, aktiverar SPI, s�tter modul till slave.

	DDRD = 0x30; //utg�ngar p� PD4 och PD5
	PORTD = 0; //test
	PORTD = 0x20; //test
	PORTD = 0; //test
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