/*
 * Communication.c
 * F�rfattare: denbe220, henba136, mikiv293, linka950
 * 2014-06-05
 * version 1.0
 */ 

#define F_CPU (18432000L) //definerar klockfrekvens f�r att _delay_us ska funka

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "../CommonLibrary/Common.h"
#include "Communication_map.h"
#include "Communication_SPI.h"
#include "Firefly.h"
#include "Communication_init.h"
int test_variable_e = 0;

//----------------MAIN--------------//
int main(void)
{
	_delay_ms(1);

	SPI_Master_init();
	USART_init();
	
	
	if((PINA & (1<<PINA4)) == MANUAL_DECISIONS_ACTIVATED)
	{
		COMMON_SET_PIN(PORTA,PORTA7); //Just nu r�d lampa lyser f�r manuellt l�ge
		sei();
		while(1){}
	}
	else
	{
		Timer_init();
		Init_button_search();
		Init_button_deliver();
		Map_init();
		COMMON_CLEAR_BIT(UCSR1B, RXCIE1); //st�nger av avbrott fr�n USART
		sei();
			
		// Startar map-kod

		//SPI_map_send_id_byte_to_sensor(ID_BYTE_GIVE_IR_SENSOR_DATA);
		SPI_map_send_command_to_steering(ID_BYTE_AUTO_DECISIONS, COMMAND_STOP);
		_delay_ms(1);
		while(!SPI_map_should_handle_new_sensor_data())
		{
		}
		_delay_ms(1);
		Create_origin(What_is_open(communication_sensor_data[0], communication_sensor_data[2], communication_sensor_data[4])); // 0,0	while(1)
		enable_node_editing = FALSE;
		//SPI_map_send_command_to_steering(ID_BYTE_AUTO_DECISIONS, COMMAND_FORWARD);
		//SPI_map_send_id_byte_to_sensor(ID_BYTE_GIVE_IR_SENSOR_DATA);
		_delay_ms(1);
		while(1)
		{
			_delay_ms(1);
			if(SPI_map_should_handle_new_sensor_data())
			{
				//test_variable_e++;
				Update_map(communication_sensor_data[4], communication_sensor_data[0], communication_sensor_data[2], communication_sensor_data[1], communication_sensor_data[3]);
				
				//_delay_ms(50);
				//SPI_map_send_id_byte_to_sensor(ID_BYTE_GIVE_IR_SENSOR_DATA);
			}
		}
	}
	
	
	
	/*
	// Startar testreglering
	SPI_map_send_command_to_steering(ID_BYTE_AUTO_DECISIONS, COMMAND_FORWARD);
    while(1)
    {
		//SPI_map_send_id_byte_to_sensor(ID_BYTE_GIVE_IR_SENSOR_DATA);
		_delay_ms(50);
    }
	*/
	
	return 0;
}

