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