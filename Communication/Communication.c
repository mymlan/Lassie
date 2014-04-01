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
	DDRA = 0x8A; //S�tter PA1, PA3 och PA7 till utg�ngar (f�r lamprona)
	DDRB = 0xB8; //S�tter SCK, MOSI, och SS till utg�ngar
	SPCR = 0xC0; //Aktiverar avbrott fr�n SPI, aktiverar SPI, s�tter modul till master. 
	SPSR = 0x01; //S�tter SCK till fosc/2
	
    while(1)
    {
        //TODO:: Please write your application code 
    }
}

int Send_data_to_sensor(unsigned char address_byte, unsigned char data) //hur g�ra om det �r flera bytes vi vill skicka?
{
	PORTB = (1<<PB4); //s�tter SS l�g
	Master_transmit_data(address_byte); //Skickar adressbyten till sensor
	//v�nta p� att sensor l�st adressbiten och lagt in den info som sensor vill skicka i sitt SPDR
	int number_of_bytes_in_data = 1; // r�kna anta bytes, alt 
	while(!(number_of_bytes_in_data == 0))
	{
		Master_transmit_data(data_byte);	
		number_of_bytes_in_data = number_of_bytes_in_data - 1;
	}
	PORTB = (0<<PB4);
	return 0;
}

//funktion som �vers�tter map adress_byte vad int number_of_bytes_in_data blir

void Master_transmit_data(unsigned char data_to_transmitt) //int eller void?
{
	SPDR = data_to_transmitt; //startar �verf�ringen
	while(!(SPSR & (1<<SPIF))) //v�nta p� att �verf�ringen ska bli klar
	{
		;
	}
}