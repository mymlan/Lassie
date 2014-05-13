
#define F_CPU (18432000L)

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "../CommonLibrary/Common.h"
#include "Communication_SPI.h"
#include "Firefly.h"

static volatile uint8_t error;

static volatile uint8_t communication_has_recieved_sensor_data;
static volatile uint8_t communication_has_recieved_distance;
static volatile uint8_t communication_has_recieved_rotation_finished;
static volatile uint8_t communication_has_recieved_rfid;
static volatile int8_t times_until_send_sensor_data_to_PC;

void SPI_Master_init(void)
{
	DDRA = 0x8A; //Sätter PA1, PA3 och PA7 till utgångar (för lamporna)
	DDRB = 0xB8; //Sätter SCK, MOSI, och SS till utgångar
	SPCR = 0x51; //Aktiverar avbrott från SPI, aktiverar SPI, sätter modul till master. 0x50
	SPSR = 0x00; //Sätter SCK till fosc/4
	COMMON_SET_PIN(PORTB, PORTB4); //initierar SS till hög
	COMMON_SET_PIN(PORTB, PORTB5); //initierar SS till hög
	//Register för att möjliggöra PCINT5
	PCMSK0 = 0x20;
	PCICR = 0x01;
	PCIFR = 0x01;
	
	error = 0;
	communication_distance = 0;
	rfid_id = 0;
	
	communication_has_recieved_sensor_data = 0;
	communication_has_recieved_distance = 0;
	communication_has_recieved_rotation_finished = 0;
	communication_has_recieved_rfid = 0;
	times_until_send_sensor_data_to_PC = NUMBER_OF_SENSOR_DATA_TO_SKIP_TO_SEND_TO_PC;
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

static void SPI_Master_recieve_ir_sensor_data()
{
	for(int8_t i = (NUMBER_OF_BYTES_IR_SENSOR_DATA - 1); i >= 0; i--)
	{
		communication_sensor_data[i] = SPI_Master_recieve_data_byte_from_sensor();
	}
}

// Avbrottsvektor som går hög då sensor har något att skicka. (röd flagga)
ISR(PCINT0_vect)
{
	uint8_t byte_from_SPI = SPI_Master_recieve_data_byte_from_sensor();
	switch (byte_from_SPI)
	{
		case ID_BYTE_IR_SENSOR_DATA:
			SPI_Master_recieve_ir_sensor_data();
			SPI_Master_send_sensor_data_to_steering(communication_sensor_data);
			if(times_until_send_sensor_data_to_PC == 0)
			{
				USART_send_sensor_data_to_PC(communication_sensor_data);
				times_until_send_sensor_data_to_PC = NUMBER_OF_SENSOR_DATA_TO_SKIP_TO_SEND_TO_PC;
			}
			else
			{
				times_until_send_sensor_data_to_PC--;
			}
			communication_has_recieved_sensor_data = 1;
			break;
		case ID_BYTE_DISTANCE:
			communication_distance = SPI_Master_recieve_data_byte_from_sensor();
			USART_send_byte_to_PC(ID_BYTE_DISTANCE, communication_distance);
			communication_has_recieved_distance = 1;
			break;
		case ID_BYTE_ROTATION_FINISHED:
			SPI_Master_send_command_to_steering(ID_BYTE_AUTO_DECISIONS, COMMAND_STOP);
			communication_has_recieved_rotation_finished = 1;
			break;
		case ID_BYTE_FOUND_RFID:
			rfid_id = SPI_Master_recieve_data_byte_from_sensor();
			USART_send_byte_to_PC(ID_BYTE_FOUND_RFID, rfid_id);
			communication_has_recieved_rfid = 1;
		default:
			error = 1;
			break;
	}
}

uint8_t SPI_map_should_handle_new_sensor_data(void)
{
	uint8_t result = communication_has_recieved_sensor_data;
	communication_has_recieved_sensor_data = 0;
	return result;
}

uint8_t SPI_map_should_handle_new_distance(void)
{
	uint8_t result = communication_has_recieved_distance;
	communication_has_recieved_distance = 0;
	return result;
}

uint8_t SPI_map_should_handle_rotation_finished(void)
{
	uint8_t result = communication_has_recieved_rotation_finished;
	communication_has_recieved_rotation_finished = 0;
	return result;
}

uint8_t SPI_map_should_handle_rfid(void)
{
	uint8_t result = communication_has_recieved_rfid;
	communication_has_recieved_rfid = 0;
	return result;
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

void SPI_Master_send_id_byte_to_sensor(uint8_t id_byte)
{
	COMMON_CLEAR_PIN(PORTB, PORTB4);
	SPI_Master_transmit_data_byte(id_byte); //Skickar adressbyten till sensor
	COMMON_SET_PIN(PORTB, PORTB4);
}

void SPI_map_send_id_byte_to_sensor(uint8_t id_byte)
{
	cli();
	COMMON_CLEAR_PIN(PORTB, PORTB4);
	SPI_Master_transmit_data_byte(id_byte); //Skickar adressbyten till sensor
	COMMON_SET_PIN(PORTB, PORTB4);
	sei();
}

// Används endast i avbrottsvektor ovan, skickar vidare sensordata till styr
void SPI_Master_send_sensor_data_to_steering(uint8_t *data_ptr)
{
	uint8_t number_of_bytes_in_data = NUMBER_OF_BYTES_IR_SENSOR_DATA;
	
	COMMON_CLEAR_PIN(PORTB, PORTB3);
	SPI_Master_transmit_data_byte(ID_BYTE_IR_SENSOR_DATA);
	for(int8_t i = (number_of_bytes_in_data - 1); i >= 0; i--)
	{
		SPI_Master_transmit_data_byte(data_ptr[i]);
	}
	COMMON_SET_PIN(PORTB, PORTB3);
}

void SPI_Master_send_command_to_steering(uint8_t id_byte, uint8_t command)
{	
	COMMON_CLEAR_PIN(PORTB, PORTB3);
	SPI_Master_transmit_data_byte(id_byte);
	SPI_Master_transmit_data_byte(command);
	COMMON_SET_PIN(PORTB, PORTB3);
}

void SPI_map_send_command_to_steering(uint8_t id_byte, uint8_t command)
{
	cli();
	COMMON_CLEAR_PIN(PORTB, PORTB3);
	SPI_Master_transmit_data_byte(id_byte);
	SPI_Master_transmit_data_byte(command);
	COMMON_SET_PIN(PORTB, PORTB3);
	sei();
}
