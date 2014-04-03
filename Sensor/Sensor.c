/*
 * Sensor.c
 *
 * Created: 2014-03-29 14:22:07
 *  Author: HuGu
 */ 


#include <avr/io.h>
#include <avr/interrupt.h>


uint8_t sensor1, sensor2, sensor3, sensor4, sensor5;

void init_ports(){
	DDRA = 0xFF;
	PORTB = 0xff;
}

void Slave_SPI_init()
{
	DDRB = 0x40; //Sätter MISO till utgång
	SPCR = 0xC0; //Aktiverar avbrott från SPI, aktiverar SPI, sätter modul till slave.
	SPSR = 0x01; //Sätter SCK till fosc/2
}

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
	
	Slave_SPI_init();
	
	sei();
	
	ADCSRA | 1<<ADSC;
	
	unsigned char byte_from_SPI = 0x00;
	volatile int new_byte_arrived_SPI = 0;
	
	while(1)
	{
		if(new_byte_arrived_SPI == 1)
		{
			Dumfunk(byte_from_SPI);
		}
	}
	
}



