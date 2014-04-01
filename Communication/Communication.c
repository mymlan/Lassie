/*
 * Communication.c
 *
 * Created: 2014-03-29 14:10:04
 *  Author: Mymlan
 */ 


#include <avr/io.h>
#include "../CommonLibrary/Common.h"

int main(void)
{
	COMMON_SET_PIN(PORTA, PORTA0);
	DDRA = 0x8A; //Sätter PA1, PA3 och PA7 till utgångar (för lamprona)
	DDRB = 0xB8; //Sätter SCK, MOSI, och SS till utgångar
	SPCR = 0xC0; //Aktiverar avbrott från SPI, aktiverar SPI, sätter modul till master. 
	SPSR = 0x01; //Sätter SCK till fosc/2
	
    while(1)
    {
        //TODO:: Please write your application code 
    }
}

int Send_data_to_sensor(unsigned char address_byte, unsigned char data_byte) //hur göra om det är flera bytes vi vill skicka?
{
	PORTB = (1<<PB3); //sätter SS låg
	Master_transmit_data(address_byte); //Skickar adressbyten till sensor
	//vänta på att sensor läst adressbiten och lagt in den info som sensor vill skicka i sitt SPDR
	Master_transmit_data(data_byte); 
	PORTB = (0<<PB3);
	return 0;
}

void Master_transmit_data(unsigned char data_to_transmitt) //int eller void?
{
	SPDR = data_to_transmitt; //startar överföringen
	while(!(SPSR & (1<<SPIF))) //vänta på att överföringen ska bli klar
	{
		;
	}
}