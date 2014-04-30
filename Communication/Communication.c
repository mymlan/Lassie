
#define F_CPU (18432000L) //definerar klockfrekvens för att _delay_us ska funka

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "../CommonLibrary/Common.h"
#include "Communication_SPI.h"


int main(void)
{
	COMMON_SET_PIN(PORTA, PORTA7);

	SPI_Master_init();
	sei();
	
	//SPI_Master_send_to_sensor(ID_BYTE_GIVE_DISTANCE);
	
	/*
	uint8_t test_sensor_data[7];
	
	test_sensor_data[0] = 10;//plockar ut värdet, alt stoppar in. 0-5
	test_sensor_data[1] = 11;
	test_sensor_data[2] = 12;
	test_sensor_data[3] = 13;
	test_sensor_data[4] = 14;
	test_sensor_data[5] = 15;
	test_sensor_data[6] = 16;
	
	SPI_Master_send_to_steering(ID_BYTE_IR_SENSOR_DATA, test_sensor_data);
	*/
	/*SPI_Master_send_command_to_steering(ID_BYTE_MANUAL_DECISIONS, COMMAND_FORWARD);
	_delay_ms(1000);
	SPI_Master_send_command_to_steering(ID_BYTE_MANUAL_DECISIONS, COMMAND_BACKWARD);
	_delay_ms(1000);
	SPI_Master_send_command_to_steering(ID_BYTE_AUTO_DECISIONS, COMMAND_ROTATE_RIGHT);
	_delay_ms(1000);
	SPI_Master_send_command_to_steering(ID_BYTE_AUTO_DECISIONS, COMMAND_ROTATE_LEFT);
	_delay_ms(1000);
	SPI_Master_send_command_to_steering(ID_BYTE_AUTO_DECISIONS, COMMAND_CLOSE_CLAW);
	_delay_ms(1000);
	SPI_Master_send_command_to_steering(ID_BYTE_AUTO_DECISIONS, COMMAND_OPEN_CLAW);
	_delay_ms(1000);
	SPI_Master_send_command_to_steering(ID_BYTE_MANUAL_DECISIONS, COMMAND_TURN_RIGHT);
	_delay_ms(1000);
	SPI_Master_send_command_to_steering(ID_BYTE_MANUAL_DECISIONS, COMMAND_TURN_LEFT);
	_delay_ms(1000);
	SPI_Master_send_command_to_steering(ID_BYTE_MANUAL_DECISIONS, COMMAND_STOP);	*/
	
    while(1)
    {
        ;
    }
	return 0;
}

