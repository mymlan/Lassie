

#define F_CPU 16000000UL

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
	Initialize_interrupt();
	Initialize_pwm();
	
	// Dessa 3 rader �r till f�r att kolla att RESET fungerar
	//Open_claw();
	//Delay_seconds(1);
	//Close_claw();
	
	// Anv�nd avborttsknappen f�r att starta testprogram
	// Koden skrivs Testprogram()
	
	Loop();
	
	return 0;
}