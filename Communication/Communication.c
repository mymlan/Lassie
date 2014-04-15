
#define F_CPU (16000000L) //definerar klockfrekvens f�r att _delay_us ska funka

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "../CommonLibrary/Common.h"

static volatile uint8_t has_recieved_sensor_data;
static volatile uint8_t has_recieved_distance;
static volatile uint8_t has_recieved_angle;
static volatile uint8_t byte_from_SPI;
static volatile uint8_t error;

static volatile uint8_t sensor_data[6];//pekaren heter sensor_data

static void Master_SPI_init()
{
		DDRA = 0x8A; //S�tter PA1, PA3 och PA7 till utg�ngar (f�r lamprona)
		DDRB = 0xB8; //S�tter SCK, MOSI, och SS till utg�ngar
		DDRD = 0x30;
		SPCR = 0x50; //Aktiverar avbrott fr�n SPI, aktiverar SPI, s�tter modul till master. 0x50
		SPSR = 0x00; //S�tter SCK till fosc/4
		COMMON_SET_PIN(PORTB, PORTB4); //initierar SS till h�g
		
		//Register f�r att m�jligg�ra PCINT5
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
* Skiftar en byte i register mellan master och slave. V�ntar p� att �verf�ring blir klar.
* Retunerar SPDR MISO
*/
static uint8_t Master_recieve_data_byte()
{
	COMMON_CLEAR_PIN(PORTB, PORTB4);
	SPDR = 0x0A; //Master m�ste l�gga n�got i SPDR f�r att starta �verf�ringen
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
*  Skiftar en byte i register mellan master och slave. V�ntar p� att �verf�ring blir klar.
*  MOSI
*/
static void Master_transmit_data_byte(uint8_t data_byte)
{
	SPDR = data_byte;
	while(!(SPSR & (1<<SPIF))){}
}



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

void Master_send(uint8_t id_byte, volatile uint8_t *data_ptr)
{
	uint8_t number_of_bytes_in_data;
	
	switch (id_byte)
	{
		case ID_BYTE_SENSOR_DATA:
			number_of_bytes_in_data = 6;
			break;
		default:
			break;
	}
	
	COMMON_CLEAR_PIN(PORTB, PORTB4);
	Master_transmit_data_byte(id_byte);
	_delay_us(10);
	while(!(number_of_bytes_in_data == 0))
	{
		Master_transmit_data_byte(data_ptr[(number_of_bytes_in_data - 1)]);
		_delay_us(10);
		number_of_bytes_in_data = number_of_bytes_in_data - 1;
	}
	
	COMMON_SET_PIN(PORTB, PORTB4);
}

int main(void)
{
	COMMON_SET_PIN(PORTA, PORTA7);

	Master_SPI_init();
	sei();
	
	
	sensor_data[0] = 0;//plockar ut v�rdet, alt stoppar in. 0-5
	sensor_data[1] = 1;
	sensor_data[2] = 2;
	sensor_data[3] = 3;
	sensor_data[4] = 4;
	sensor_data[5] = 5;
	
	Master_send(0x01, sensor_data);
	
    while(1)
    {
        ;
    }
	return 0;
}

