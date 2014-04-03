/*
 * Communication.c
 *
 * Created: 2014-03-29 14:10:04
 *  Author: MikaLina
 */ 

#include <avr/io.h>
#include "../CommonLibrary/Common.h"

void Master_SPI_init()
{
		DDRA = 0x8A; //S�tter PA1, PA3 och PA7 till utg�ngar (f�r lamprona)
		DDRB = 0xB8; //S�tter SCK, MOSI, och SS till utg�ngar
		SPCR = 0xD0; //Aktiverar avbrott fr�n SPI, aktiverar SPI, s�tter modul till master.
		SPSR = 0x01; //S�tter SCK till fosc/2
		
}

/* void Master_transmit_data_byte(unsigned char data_byte)
*  Skiftar en byte i register mellan master och slave. V�ntar p� att �verf�ring blir klar.
*  MOSI
*/
void Master_transmit_data_byte(unsigned char data_byte)
{
	SPDR = data_byte;
	while(!(SPSR & (1<<SPIF)))
	{
		;
	}
}


/* int Master_recieve_data_byte()
* Skiftar en byte i register mellan master och slave. V�ntar p� att �verf�ring blir klar.
* Retunerar SPDR
*/
unsigned char Master_recieve_data_byte()
{
	SPDR = 0x00; //Master m�ste l�gga n�got i SPDR f�r att starta �verf�ringen
	while(!(SPSR & (1<<SPIF)))
	{
		;
	}
	return SPDR;
}

/* Send_address_to_slave(unsigned char address_byte)
*  Skickar adress-byte fr�n master till sensor_slave
*/
int Send_address_to_sensor(unsigned char address_byte) 
{
	PORTB = (0<<PB4); //s�tter SS l�g,
	Master_transmit_data_byte(address_byte); //Skickar adressbyten till sensor
	PORTB = (1<<PB4);
	
	volatile int delay = 0;
	while(delay<100) //f�rdr�jnig s� att styr hinner tolka adressbyten, siffran 100 �r taget ur luften
	{
		delay = delay + 1;
	}
	return 0;
}


int main(void)
{
	COMMON_SET_PIN(PORTA, PORTA0);

	Master_SPI_init();
	Send_address_to_sensor(0x02);

    while(1)
    {
        ;
    }
}
