
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

static volatile uint8_t sensor_data[6];//pekaren heter sensor_data
static volatile uint8_t distance;
static volatile uint8_t angle;

static void SPI_Master_init()
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
		
		has_recieved_sensor_data = 0;
		has_recieved_distance = 0;
		has_recieved_angle = 0;	
		byte_from_SPI = 0;	
		error = 0;
		
		// sensor_data = ?;
		distance = 0;
		angle = 0;
}

/* static uint8_t SPI_Master_recieve_data_byte_from_sensor()
* Skiftar en byte i register mellan master och slave. Väntar på att överföring blir klar.
* Retunerar SPDR, MISO
*/
static uint8_t SPI_Master_recieve_data_byte_from_sensor()
{
	COMMON_CLEAR_PIN(PORTB, PORTB4);
	SPDR = 0x0A; //Master måste lägga något i SPDR för att starta överföringen
	while(!(SPSR & (1<<SPIF))){}
	COMMON_SET_PIN(PORTB, PORTB4);
	return SPDR;
}

static void SPI_Master_recieve_sensor_data()
{
	_delay_us(10);
	uint8_t number_of_bytes_in_data = 6;
	while(!(number_of_bytes_in_data == 0))
	{
		sensor_data[(number_of_bytes_in_data - 1)] = SPI_Master_recieve_data_byte_from_sensor();
		_delay_us(10);
		number_of_bytes_in_data = number_of_bytes_in_data - 1;		
	}
	sei();
}
static void SPI_Master_recieve_distance()
{
	_delay_us(10);
	distance = SPI_Master_recieve_data_byte_from_sensor();
	_delay_us(10);
	sei();
}
static void SPI_Master_recieve_angle()
{
	_delay_us(10);
	angle = SPI_Master_recieve_data_byte_from_sensor();
	_delay_us(10);
	sei();
}

// Avbrottsvektor som går hög då sensor har något att skicka. (röd flagga)
ISR(PCINT0_vect)
{
	cli(); // Avaktiverar avbrott under skickning
	byte_from_SPI = SPI_Master_recieve_data_byte_from_sensor();
	switch (byte_from_SPI) 
	{
		case ID_BYTE_SENSOR_DATA:
		SPI_Master_recieve_sensor_data();
		has_recieved_sensor_data = 1;
		break;
		case ID_BYTE_DISTANCE:
		SPI_Master_recieve_distance();
		has_recieved_distance = 1;
		break;
		case ID_BYTE_ANGLE:
		SPI_Master_recieve_angle();
		has_recieved_angle = 1;
		default:
		error = 1;
		PORTD = 0;
		PORTD = 0x20;
		PORTD = 0;
		break;
	}
		
}

uint8_t SPI_Master_has_new_sensor_data(void)
{
	uint8_t result = has_recieved_sensor_data;
	has_recieved_sensor_data = 0;
	return result;
}
uint8_t SPI_Master_has_new_distance(void)
{
	uint8_t result = has_recieved_distance;
	has_recieved_distance = 0;
	return result;
}
uint8_t SPI_Master_has_new_angle(void)
{
	uint8_t result = has_recieved_angle;
	has_recieved_angle = 0;
	return result;
}

/* void SPI_Master_transmit_data_byte(unsigned char data_byte)
*  Skiftar en byte i register mellan master och slave. Väntar på att överföring blir klar.
*  MOSI
*/
static void SPI_Master_transmit_data_byte(uint8_t data_byte)
{
	SPDR = data_byte;
	while(!(SPSR & (1<<SPIF))){}
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

void SPI_Master_send_to_steering(uint8_t id_byte, volatile uint8_t *data_ptr)
{
	volatile uint8_t number_of_bytes_in_data = 0;
	
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
	_delay_us(10);
	while(!(number_of_bytes_in_data == 0))
	{
		SPI_Master_transmit_data_byte(data_ptr[(number_of_bytes_in_data - 1)]);
		_delay_us(10);
		number_of_bytes_in_data = number_of_bytes_in_data - 1;
	}
	COMMON_SET_PIN(PORTB, PORTB3);
	sei();
}

int main(void)
{
	COMMON_SET_PIN(PORTA, PORTA7);

	SPI_Master_init();
	sei();
	
	
	sensor_data[0] = 0;//plockar ut värdet, alt stoppar in. 0-5
	sensor_data[1] = 1;
	sensor_data[2] = 2;
	sensor_data[3] = 3;
	sensor_data[4] = 4;
	sensor_data[5] = 5;
	
	SPI_Master_send_to_steering(0x01, sensor_data);
    while(1)
    {
        ;
    }
	return 0;
}

