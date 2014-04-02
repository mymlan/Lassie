/*
 * Communication.c
 *
 * Created: 2014-03-29 14:10:04
 *  Author: MikaLina
 */ 

#include <avr/io.h>
#include "../CommonLibrary/Common.h"

void SPI_init()
{
		DDRA = 0x8A; //S�tter PA1, PA3 och PA7 till utg�ngar (f�r lamprona)
		DDRB = 0xB8; //S�tter SCK, MOSI, och SS till utg�ngar
		SPCR = 0xD0; //Aktiverar avbrott fr�n SPI, aktiverar SPI, s�tter modul till master.
		SPSR = 0x01; //S�tter SCK till fosc/2
}

/*//hur g�ra om det �r flera bytes vi vill skicka?
int number_of_bytes_in_data = 1; // r�kna anta bytes, alt
while(!(number_of_bytes_in_data == 0))
{
	Master_transmit_data(data_byte);
	number_of_bytes_in_data = number_of_bytes_in_data - 1;
}*/

/* void Master_transmit_data_byte(unsigned char data_byte)
*  Skiftar en byte i register mellan master och slave. V�ntar p� att �verf�ring blir klar.
*/
void Master_transmit_data_byte(unsigned char data_byte)
{
	SPDR = data_byte;
	while(!(SPSR & (1<<SPIF)))
	{
		;
	}
}

/* Send_address_to_slave(unsigned char address_byte)
*  Skickar adress-byte fr�n master till sensor_slave
*/
int Send_address_to_sensor(unsigned char address_byte) 
{
	PORTB = (0<<PB4); //s�tter SS l�g
	Master_transmit_data_byte(address_byte); //Skickar adressbyten till sensor
	PORTB = (1<<PB4);
	return 0;
}

/* Send_data_byte_to_steering(unsigned char address_byte, unsigned char data_byte) 
* Skickar en byte till styr, till exempel d� vi skickar styrbeslut.
*/

int Send_data_byte_to_steering(unsigned char address_byte, unsigned char data_byte) 
{
	PORTB = (0<<PB3); //s�tter SS l�g
	Master_transmit_data_byte(address_byte); //Skickar adressbyten till styr
	volatile int delay = 0;
	while(delay<100) //f�rdr�jnig s� att styr hinner tolka adressbyten, siffran 100 �r taget ur luften
	{
		delay = delay + 1;
	}
	delay = 0;
	Master_transmit_data_byte(data_byte); //Skickar databyten till styr
	PORTB = (1<<PB3); //s�tter SS h�g
	return 0;
}

int main(void)
{
	COMMON_SET_PIN(PORTA, PORTA0);

	SPI_init();
	Send_address_to_sensor(0x02);
	Send_data_byte_to_steering(0x07,0x01);
    while(1)
    {
        //TODO:: Please write your application code 
    }
}
