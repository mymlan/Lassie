/*
 * Communication.c
 *
 * Created: 2014-03-29 14:10:04
 *  Author: MikaLina
 */ 
#define F_CPU (16000000L) //definerar klockfrekvens f�r att _delay_us ska funka

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
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
static void Master_transmit_data_byte(uint8_t data_byte)
{
	SPDR = data_byte;
	while(!(SPIF == 1)){}
}

/* int Master_recieve_data_byte()
* Skiftar en byte i register mellan master och slave. V�ntar p� att �verf�ring blir klar.
* Retunerar SPDR MISO
*/
/*
static uint8_t Master_recieve_data_byte()
{
	SPDR = 0x00; //Master m�ste l�gga n�got i SPDR f�r att starta �verf�ringen
	while(!(SPIF == 1)){}
	return SPDR;
}
*/

/* Send_address_to_sensor(unsigned char address_byte)
*  Skickar adress-byte fr�n master till sensor_slave
*/
void Master_send_to_sensor(uint8_t address_byte) 
{
	COMMON_CLEAR_PIN(PORTB, PORTB4);
	Master_transmit_data_byte(address_byte); //Skickar adressbyten till sensor
	COMMON_SET_PIN(PORTB, PORTB4);
	_delay_us(10);
}

int main(void)
{
	COMMON_SET_PIN(PORTA, PORTA1);

	Master_SPI_init();
	sei();
	Master_send_to_sensor(ID_BYTE_GIVE_ANGLE);

    while(1)
    {
        ;
    }
}
