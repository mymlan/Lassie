/*
 * Communication.c
 *
 * Created: 2014-03-29 14:10:04
 *  Author: MikaLina
 */ 
#define F_CPU (16000000L) //definerar klockfrekvens för att _delay_us ska funka

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "../CommonLibrary/Common.h"

static volatile uint8_t has_recieved_sensor_data;
static volatile uint8_t has_recieved_distance;
static volatile uint8_t has_recieved_angle;
static volatile uint8_t byte_from_SPI;
static volatile uint8_t error;
/*
static volatile uint8_t test1 = 0;
static volatile uint8_t test2 = 0;
static volatile uint8_t test[2] = {0,0};
*/
static void Master_SPI_init()
{
		DDRA = 0x8A; //Sätter PA1, PA3 och PA7 till utgångar (för lamprona)
		DDRB = 0xB8; //Sätter SCK, MOSI, och SS till utgångar
		DDRD = 0x30;
		SPCR = 0x50; //Aktiverar avbrott från SPI, aktiverar SPI, sätter modul till master. 0x50
		SPSR = 0x00; //Sätter SCK till fosc/4
		COMMON_SET_PIN(PORTB, PORTB4); //initierar SS till hög
		
		//Register för att möjliggöra PCINT5
		PCMSK0 = 0x20;
		PCICR = 0x01;
		PCIFR = 0x01;
		
		has_recieved_sensor_data = 0;
		has_recieved_distance = 0;
		has_recieved_angle = 0;	
		byte_from_SPI = 0;	
		error = 0;
}

/* int Master_recieve_data_byte()
* Skiftar en byte i register mellan master och slave. Väntar på att överföring blir klar.
* Retunerar SPDR MISO
*/

static uint8_t Master_recieve_data_byte()
{
	COMMON_CLEAR_PIN(PORTB, PORTB4);
	SPDR = 0x0A; //Master måste lägga något i SPDR för att starta överföringen
	while(!(SPSR & (1<<SPIF))){}
	COMMON_SET_PIN(PORTB, PORTB4);
	return SPDR;
}

ISR(PCINT0_vect)
{
	byte_from_SPI = Master_recieve_data_byte(); //byte_from_SPI = Master...();
	switch (byte_from_SPI) 
	{
		case ID_BYTE_SENSOR_DATA:
		has_recieved_sensor_data = 1;
		break;
		case ID_BYTE_DISTANCE:
		has_recieved_distance = 1;
		break;
		case ID_BYTE_ANGLE:
		has_recieved_angle = 1;
		default:
		error = 1;
		PORTD = 0;
		PORTD = 0x20;
		PORTD = 0;
		break;
	}
		
}


/* void Master_transmit_data_byte(unsigned char data_byte)
*  Skiftar en byte i register mellan master och slave. Väntar på att överföring blir klar.
*  MOSI
*/

static void Master_transmit_data_byte(uint8_t data_byte)
{
	SPDR = data_byte;
	while(!(SPSR & (1<<SPIF))){}
}



/* Send_address_to_sensor(unsigned char address_byte)
*  Skickar adress-byte från master till sensor_slave
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
	COMMON_SET_PIN(PORTA, PORTA7);

	Master_SPI_init();
	sei();
	//Master_send_to_sensor(0x09);

	/*
	test1 = 4;
	test2 = 5;
	test[0] = test1;
	test[1] = test2;
	int *test_ptr;
	test_ptr = &test[0];
	error = test_ptr->test2;
	*/
    while(1)
    {
        ;
    }
}
