

//#define F_CPU 16000000UL
#define F_CPU 18432000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "Steering_SPI.h"
#include "Steering_functions.h"

//----------------MAIN--------------//
int main(void)
{
	// Initiering
	SPI_steering_init();
	//Initialize_interrupt();
	//Initialize_pwm();
	sei();
	
	
	Loop();
	
	return 0;
}