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

/* unsigned char Interprete_address_byte(unsigned char address_byte)
* funktionen g�r inget vettigt i nul�get (2014-04-02) Skrev den f�r att inte
* f� kompileringsfel pga oanv�nd parameter address_byte. Sensor vill troligtvis ha 
* en liknande funktion n�gon stans
*/
int Interprete_address_byte(unsigned char address_byte)
{
	if(address_byte == 0x02) //komm vill ha avverkat avst�nd
	{
		return 1;
	}
	else if(address_byte == 0x04) //starta vinkelhastighetssensorn
	{
		return 2;
	}
	else if(address_byte == 0x05) //Komm vill ha vriden vinkel
	{
		return 3;
	}
	else
	{
		return 0;
	}
}

ISR(SPI_STC_vect) //Den avbrotsrutin som sensorn g�r in i d� komm skickat data.
{
	unsigned char address_byte;
	
	while(!(SPSR & (1<<SPIF))) //loop som v�ntar p� att mottagningen ska bli klar
	{
		;
	}
	
	address_byte = SPDR;
	
	Interprete_address_byte(address_byte);
	
}