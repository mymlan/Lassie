
#define F_CPU (16000000L)

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "../CommonLibrary/Common.h"
#include "Communication_SPI.h"

static volatile uint8_t error;

void SPI_Master_init(void)
{
	DDRA = 0x8A; //Sätter PA1, PA3 och PA7 till utgångar (för lamporna)
	DDRB = 0xB8; //Sätter SCK, MOSI, och SS till utgångar
	DDRD = 0x30;
	SPCR = 0x50; //Aktiverar avbrott från SPI, aktiverar SPI, sätter modul till master. 0x50
	SPSR = 0x00; //Sätter SCK till fosc/4
	COMMON_SET_PIN(PORTB, PORTB4); //initierar SS till hög
	
	//Register för att möjliggöra PCINT5
	PCMSK0 = 0x20;
	PCICR = 0x01;
	PCIFR = 0x01;
	
	error = 0;
}

/* static uint8_t SPI_Master_recieve_data_byte_from_sensor(void)
* Skiftar en byte i register mellan master och slave. Väntar på att överföring blir klar.
* Retunerar SPDR, MISO
*/
static uint8_t SPI_Master_recieve_data_byte_from_sensor(void)
{
	COMMON_CLEAR_PIN(PORTB, PORTB4);
	SPDR = 0x0A; //Master måste lägga något i SPDR för att starta överföringen
	while(!(SPSR & (1<<SPIF))){}
	COMMON_SET_PIN(PORTB, PORTB4);
	_delay_us(10);
	return SPDR;
}

static void SPI_Master_recieve_sensor_data(uint8_t *sensor_data)
{
	for(int8_t i = (NUMBER_OF_BYTES_SENSOR_DATA - 1); i >= 0; i--)
	{
		sensor_data[i] = SPI_Master_recieve_data_byte_from_sensor();
	}
}

// Avbrottsvektor som går hög då sensor har något att skicka. (röd flagga)
ISR(PCINT0_vect)
{
	cli(); // Avaktiverar avbrott under skickning
	uint8_t byte_from_SPI = SPI_Master_recieve_data_byte_from_sensor();
	switch (byte_from_SPI)
	{
		case ID_BYTE_SENSOR_DATA:
		{
			uint8_t sensor_data[6];
			SPI_Master_recieve_sensor_data(sensor_data);
			SPI_Master_send_to_steering(ID_BYTE_SENSOR_DATA, sensor_data);
			// skicka till PC
			break;
		}
		case ID_BYTE_DISTANCE:
		{
			uint8_t distance = SPI_Master_recieve_data_byte_from_sensor();
			(void)distance; //löser att den inte används, gör om till void
			// skicka till PC
			break;
		}
		case ID_BYTE_ANGLE:
		{
			uint8_t angle = SPI_Master_recieve_data_byte_from_sensor();
			(void)angle; //löser att den inte används, gör om till void
			// skicka till PC
			break;
		}
		default:
		error = 1;
		PORTD = 0;
		PORTD = 0x20;
		PORTD = 0;
		break;
	}
	sei();
	
}

/* static void SPI_Master_transmit_data_byte(uint8_t data_byte)
*  Skiftar en byte i register mellan master och slave. Väntar på att överföring blir klar.
*  MOSI
*/
static void SPI_Master_transmit_data_byte(uint8_t data_byte)
{
	SPDR = data_byte;
	while(!(SPSR & (1<<SPIF))){}
	_delay_us(10);
}

/* void SPI_Master_send_to_sensor(uint8_t id_byte)
*  Skickar id_byte från Master till sensor Slave.
*  Då endast meddelanden om 1 byte ska till sensor.
*/
void SPI_Master_send_to_sensor(uint8_t id_byte)
{
	cli();
	COMMON_CLEAR_PIN(PORTB, PORTB4);
	SPI_Master_transmit_data_byte(id_byte); //Skickar adressbyten till sensor
	COMMON_SET_PIN(PORTB, PORTB4);
	_delay_us(10); //ger tid till sensor att spara undan SPDR
	sei();
}

void SPI_Master_send_to_steering(uint8_t id_byte, uint8_t *data_ptr)
{
	uint8_t number_of_bytes_in_data = 0;
	switch (id_byte)
	{
		case ID_BYTE_SENSOR_DATA:
		number_of_bytes_in_data = 6;
		break;
		case ID_BYTE_GUIDED_DECISIONS:
		case ID_BYTE_AUTO_DECISIONS:
		number_of_bytes_in_data = 1;
		break;
		default:
		error = 1;
		break;
	}
	
	cli();
	COMMON_CLEAR_PIN(PORTB, PORTB3);
	SPI_Master_transmit_data_byte(id_byte);
	for(int8_t i = (number_of_bytes_in_data - 1); i >= 0; i--)
	{
		SPI_Master_transmit_data_byte(data_ptr[i]);
	}
	COMMON_SET_PIN(PORTB, PORTB3);
	sei();
}

