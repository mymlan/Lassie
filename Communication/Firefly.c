/*
 * Firefly.c
 *
 * Created: 5/2/2014 12:00:11 PM
 *  Author: linka950
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "../CommonLibrary/Common.h"
#include "Communication_SPI.h"

static volatile uint8_t USART_avbrott;

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