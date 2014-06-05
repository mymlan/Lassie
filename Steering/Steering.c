/*
 * Steering.c 
 * Författare: denbe220, henba136
 * 2014-06-05
 * version 1.0
 */ 

#include <avr/interrupt.h>
#include "Steering_SPI.h"
#include "Steering_functions.h"

//----------------MAIN--------------//
int main(void)
{
	// Initiering
	Steering_initialize_7_segment_display_error_pins();
	Steering_initialize_pwm();
	SPI_steering_init();
	
	sei();
	
	while(1)
	{
		;
	}
	
	return 0;
}