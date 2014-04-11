/*
 * Sensor.c
 *
 * Created: 2014-03-29 14:22:07
 *  Author: HuGu
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "../CommonLibrary/Common.h"

static volatile uint8_t has_recieved_give_sensor_data = 0;
static volatile uint8_t has_recieved_give_distance = 0;
static volatile uint8_t has_recieved_start_calc_angle = 0;
static volatile uint8_t has_recieved_give_angle = 0;
static volatile uint8_t error = 0;
static volatile uint8_t byte_from_SPI = 0x00;
static volatile uint8_t SPI_test = 0x00;	

void Init_ports(){
	DDRA = 0xFF;
	PORTB = 0xFF;
	DDRB = 0x40; //Sätter MISO till utgång
	SPCR = 0xC0; //Aktiverar avbrott från SPI, aktiverar SPI, sätter modul till slave.
	SPSR = 0x01; //Sätter SCK till fosc/2
	DDRD = 0x30;
	PORTD = 0;
	PORTD = 0x20;
	PORTD = 0;
	SPDR = 0x22;
}

ISR(SPI_STC_vect) //Den avbrotsrutin som sensorn går in i då komm skickat data.
{
	byte_from_SPI = SPDR;
	switch (byte_from_SPI)
	{
		case ID_BYTE_GIVE_SENSOR_DATA:
			has_recieved_give_sensor_data = 1;
			break;
		case ID_BYTE_GIVE_DISTANCE:
			has_recieved_give_distance = 1;
			break;
		case ID_BYTE_START_CALC_ANGLE:
			has_recieved_start_calc_angle = 1;
			break;
		case ID_BYTE_GIVE_ANGLE:
			has_recieved_give_angle = 1;
			break;
		default:
			error = 1;
			PORTD = 0;
			PORTD = 0x20;
			PORTD = 0;
			break;
	}
}

uint8_t SPI_should_give_sensor_data(void)
{
	uint8_t result = has_recieved_give_sensor_data;
	has_recieved_give_sensor_data = 0;
	return result;
}

uint8_t SPI_should_give_distance(void)
{
	uint8_t result = has_recieved_give_distance;
	has_recieved_give_distance = 0;
	return result;
}

uint8_t SPI_should_start_calc_angle(void)
{
	uint8_t result = has_recieved_start_calc_angle;
	has_recieved_start_calc_angle = 0;
	return result;
}

uint8_t SPI_should_give_angle(void)
{
	uint8_t result = has_recieved_give_angle;
	has_recieved_give_angle = 0;
	return result;
}
/*
void SPI_sensor_send(uint8_t id_byte, uint8_t *data)
{
	uint8_t number_of_bytes_in_data;
	switch (id_byte)
	{
		case ID_BYTE_SENSOR_DATA:
			number_of_bytes_in_data = 6;
			break;
		case ID_BYTE_DISTANCE:
		case ID_BYTE_ANGLE:
			number_of_bytes_in_data = 1;
			break;
	}
	cli(); //Borde hitta den som stänger av avbrott för SPI!!
	SPI_send_byte(id_byte);
	if(is_slave_ready)
	{
		while(!(number_of_bytes_in_data == 0))
		{
			SPI_send_byte();
			number_of_bytes_in_data = number_of_bytes_in_data - 1;
		}
	}	
}
*/
void SPI_send(uint8_t id_byte)
{
	cli(); //Borde hitta den som stänger av avbrott för SPI!!
	SPI_send_byte(id_byte);

}

static void SPI_send_byte(uint8_t byte)
{
	SPDR = byte;
	COMMON_SET_PIN(PORTB, PORTB0); //Avbrott till komm
}

int main(void)
{
	Init_ports();
	sei();
	SPI_send(0x03);
	
	while(1)
	{
		/*if(SPI_should_give_sensor_data())
		{
			SPI_test = 0xff;
		}*/
	}
	
}

