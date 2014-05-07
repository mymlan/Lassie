
#define F_CPU (18432000L) //definerar klockfrekvens för att _delay_us ska funka

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "../CommonLibrary/Common.h"
#include "Communication_map.h"
#include "Communication_SPI.h"
#include "Firefly.h"

int main(void)
{
	COMMON_SET_PIN(PORTA, PORTA7);

	SPI_Master_init();
	USART_init();
	sei();
	
	//Map_main(); // KOMMENTERA BORT DENNA RAD OM NI INTE TESTAR KARTAN
	
	//timer (hämta sensordata)
	/*
	while(1)
	{
		if(SPI_map_should_handle_new_sensor_data())
		{
			Update_map(communication_sensor_data[0], uint8_t sensor_front_left, uint8_t sensor_front_right, uint8_t sensor_back_left, uint8_t sensor_back_right);
		}
	}*/
	
	
	
	SPI_Master_send_command_to_steering(ID_BYTE_AUTO_DECISIONS, COMMAND_FORWARD);
    while(1)
    {
		_delay_ms(50);
		
		SPI_Master_send_id_byte_to_sensor(ID_BYTE_GIVE_IR_SENSOR_DATA);
    }
	return 0;
}

