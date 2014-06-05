/*
 * Sensor.c 
 * Författare: hugjo456, gusno119
 * 2014-06-05
 * version 1.0
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "../CommonLibrary/Common.h"

#include "Sensor_SPI.h"
#include "Sensor_cm_converter.h"

//----------------MAIN--------------//
int main(void)
{
	// Initiering
	Sensor_init_AD_converter();
	Sensor_init_analog_comparator();
	Sensor_init_USART_for_RFID();	
	Sensor_init_button_for_calibrating_angular_rate_sensor();

  	SPI_sensor_init();
	
	// Kalibrering
	Calibrate_angular_rate_sensor();
		
	sei();
	
    while(1)
	{
		;
	}
	
}
			
			