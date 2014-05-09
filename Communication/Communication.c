
#define F_CPU (18432000L) //definerar klockfrekvens för att _delay_us ska funka

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "../CommonLibrary/Common.h"
#include "Communication_map.h"
#include "Communication_SPI.h"
#include "Firefly.h"
int test_variable_e = 0;
int main(void)
{
	COMMON_SET_PIN(PORTA, PORTA7);

	SPI_Master_init();
	USART_init();
	sei();
	
	//Map_main(); // KOMMENTERA BORT DENNA RAD OM NI INTE TESTAR KARTAN
	
	//timer (hämta sensordata)
	
	
	// Startar map-kod
	SPI_Master_send_id_byte_to_sensor(ID_BYTE_GIVE_IR_SENSOR_DATA);
	while(!SPI_map_should_handle_new_sensor_data())
	{
	}
	Create_origin(What_is_open(communication_sensor_data[0], communication_sensor_data[2], communication_sensor_data[4])); // 0,0	while(1)
	
	//SPI_Master_send_command_to_steering(ID_BYTE_AUTO_DECISIONS, COMMAND_FORWARD);
	
	SPI_Master_send_id_byte_to_sensor(ID_BYTE_GIVE_IR_SENSOR_DATA);
	while(1)
	{
		if(SPI_map_should_handle_new_sensor_data())
		{
			test_variable_e++;
			SPI_Master_send_command_to_steering(ID_BYTE_AUTO_DECISIONS, COMMAND_FORWARD);
			Update_map(communication_sensor_data[4], communication_sensor_data[0], communication_sensor_data[2], communication_sensor_data[1], communication_sensor_data[3]);
			_delay_ms(50);
			SPI_Master_send_id_byte_to_sensor(ID_BYTE_GIVE_IR_SENSOR_DATA);
		}
	}
	
	
	/*
	// Startar testreglering
	SPI_Master_send_command_to_steering(ID_BYTE_AUTO_DECISIONS, COMMAND_FORWARD);
    while(1)
    {
		SPI_Master_send_id_byte_to_sensor(ID_BYTE_GIVE_IR_SENSOR_DATA);
		_delay_ms(50);
    }
	*/
	while(1){}
	return 0;
}

