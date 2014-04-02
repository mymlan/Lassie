/*
* Sensor_SPI
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include "../CommonLibrary/Common.h"


void SPI_init()
{
	DDRB = 0x40; //Sätter MISO till utgång
	SPCR = 0xC0; //Aktiverar avbrott från SPI, aktiverar SPI, sätter modul till slave.
	SPSR = 0x01; //Sätter SCK till fosc/2
}

/* unsigned char Interprete_address_byte(unsigned char address_byte)
* funktionen gör inget vettigt i nuläget (2014-04-02) Skrev den för att inte
* få kompileringsfel pga oanvänd parameter address_byte. Sensor vill troligtvis ha 
* en liknande funktion någon stans
*/
int Interprete_address_byte(unsigned char address_byte)
{
	if(address_byte == 0x02) //komm vill ha avverkat avstånd
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

ISR(SPI_STC_vect) //Den avbrotsrutin som sensorn går in i då komm skickat data.
{
	unsigned char address_byte;
	
	while(!(SPSR & (1<<SPIF))) //loop som väntar på att mottagningen ska bli klar
	{
		;
	}
	
	address_byte = SPDR;
	
	Interprete_address_byte(address_byte);
	
}