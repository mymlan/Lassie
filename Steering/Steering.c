
#include <avr/interrupt.h>
#include "Steering_SPI.h"
#include "Steering_functions.h"

//----------------MAIN--------------//
int main(void)
{
	// Initiering
	SPI_steering_init();
	Initialize_pwm();
	sei();
	
	while(1)
	{
		;
	}
	
	return 0;
}