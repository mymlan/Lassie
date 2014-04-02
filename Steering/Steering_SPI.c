/*
* Sensor_SPI
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include "../CommonLibrary/Common.h"

void SPI_init()
{
	DDRB = 0x40; //S�tter MISO till utg�ng
	SPCR = 0xC0; //Aktiverar avbrott fr�n SPI, aktiverar SPI, s�tter modul till slave.
	SPSR = 0x01; //S�tter SCK till fosc/2
}