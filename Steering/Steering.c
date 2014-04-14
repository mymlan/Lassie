
#include <avr/interrupt.h>
#include "Steering_SPI.h"


int main(void)
{
	SPI_steering_init();
	sei();

	while (1)
	{		
		;
	}
	return 0;
}