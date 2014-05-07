
#include <avr/io.h>
#include <avr/interrupt.h>
#include "../CommonLibrary/Common.h"

#include "Sensor_SPI.h"
#include "Sensor_cm_converter.h"

int main(void)
{
	USART_init();
  	init_interrupts();
	SPI_sensor_init();
	sei();
	
	//För att kunna förbättre regleringen finns följande while
	/*while(1)
	{
		if(SPI_sensor_should_give_ir_sensor_data())
		{
			SPI_sensor_send_sensor_data(ir_sensor_data);
		}
	}*/
	
    while(1)
	{
		;
	}
	
}
			
			