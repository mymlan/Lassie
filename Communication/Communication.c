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