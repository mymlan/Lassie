/*
* Sensor_SPI
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include "../Sensor/Sensor.c"



/* int Interprete_address_byte(unsigned char address_byte)
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
