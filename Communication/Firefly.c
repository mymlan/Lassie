
#define F_CPU (18432000L)

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "../CommonLibrary/Common.h"
#include "Communication_SPI.h"
#include "Firefly.h"

static volatile uint8_t USART_avbrott;
static volatile uint8_t error;

void USART_init()
{	
	DDRD = 0x08; //Sätter PD3 (USART1 transmitt pin) till utgång
	UBRR1 = 0x09; //Sätter baudrate
	
	UCSR1A = 0x00;
	UCSR1B = 0x98; //Möjliggör skickning och mottagning med USART, samt aktiverar avbrott från RXC1 flaggan
	UCSR1C = 0x06; //Anger att vi vill skicka 8 bitar.
}

ISR(USART1_RX_vect)
{
	cli();
	uint8_t command = UDR1;
	USART_avbrott = command;
	SPI_Master_send_command_to_steering(ID_BYTE_MANUAL_DECISIONS, command);
	
	sei();
}

static void USART_transmit_byte_to_PC(uint8_t data_byte)
{
	while(!(UCSR1A & (1<<UDRE1))){} //Väntar tills UDR1 är redo att ta emot ny data	
	UDR1 = data_byte;
	_delay_us(10); //Testa om vi verkligen behöver en delay
}

void USART_send_sensor_data_to_PC(uint8_t *data_ptr)
{
	uint8_t number_of_bytes_in_data = NUMBER_OF_BYTES_IR_SENSOR_DATA;
	
	cli();
	USART_transmit_byte_to_PC(ID_BYTE_IR_SENSOR_DATA);
	for(int8_t i = (number_of_bytes_in_data - 1); i >= 0; i--)
	{
		USART_transmit_byte_to_PC(data_ptr[i]);
	}
	sei();
}

void USART_send_byte_to_PC(uint8_t id_byte, uint8_t value)
{
	cli();
	USART_transmit_byte_to_PC(id_byte);
	USART_transmit_byte_to_PC(value);
	sei();
}

void USART_send_map_coordinates_to_PC(uint8_t id_byte, int8_t x_coordinate, int8_t y_coordinate)
{
	cli();
	USART_transmit_byte_to_PC(id_byte);
	USART_transmit_byte_to_PC(x_coordinate);
	USART_transmit_byte_to_PC(y_coordinate);
	sei();
}