/*
 * Sensor.c
 *
 * Created: 2014-03-29 14:22:07
 *  Author: HuGu
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "../CommonLibrary/Common.h"

static uint8_t sensor1, sensor2, sensor3, sensor4, sensor5;
static uint8_t byte_from_SPI = 0x00;
static volatile uint8_t new_byte_arrived_SPI = 0;

void init_ports(){
	DDRA = 0xFF;
	PORTB = 0xFF;
	DDRB = 0x40; //Sätter MISO till utgång
	SPCR = 0xC0; //Aktiverar avbrott från SPI, aktiverar SPI, sätter modul till slave.
	SPSR = 0x01; //Sätter SCK till fosc/2
}
/*
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
*/

ISR(SPI_STC_vect) //Den avbrotsrutin som sensorn går in i då komm skickat data.
{
	cli();
	byte_from_SPI = SPDR;
	new_byte_arrived_SPI = 1;
	sei();
}

ISR(ADC_vect)
{
	switch(ADMUX)
	{
		case 0xE0:
		sensor1 = ADCH;
		ADMUX = 0xE1;
		break;
		case 0xE1:
		sensor2 = ADCH;
		ADMUX = 0xE2;
		break;
		case 0xE2:
		sensor3 = ADCH;
		ADMUX = 0xE3;
		break;
		case 0xE3:
		sensor4 = ADCH;
		ADMUX = 0xE4;
		break;
		case 0xE4:
		sensor5 = ADCH;
		ADMUX = 0xE0;
		break;
	}
	ADCSRA | 1<<ADSC;
}

unsigned char Dumfunk(unsigned char useless)
{
	
	return useless;
}

int main(void)
{
	init_ports();
	ADCSRA |= 1<<ADPS2;
	ADMUX |= 1<<REFS0 | 1<<REFS1 | 1<<ADLAR | 2;
	ADCSRA |= 1<<ADIE;
	ADCSRA |= 1<<ADEN;
	
	sei();
	
	ADCSRA | 1<<ADSC;
	//unsigned char distance = 0x00;
	
	while(1)
	{
		if(new_byte_arrived_SPI == 1)
		{
			if(byte_from_SPI == ID_BYTE_GIVE_DISTANCE)
			{
				//request_new_distance = 1;
			}
			new_byte_arrived_SPI = 0;
		}
	}
	
}

void Send_distance(unsigned char distance)
{
	SPDR = distance;
	//SENSOR_READY_TO_TRANSMITT;
}

