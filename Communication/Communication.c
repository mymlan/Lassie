/*
 * Communication.c
 *
 * Created: 2014-03-29 14:10:04
 *  Author: MikaLina
 */ 

#include <avr/io.h>
#include "../CommonLibrary/Common.h"


/*//hur göra om det är flera bytes vi vill skicka?
int number_of_bytes_in_data = 1; // räkna anta bytes, alt
while(!(number_of_bytes_in_data == 0))
{
	Master_transmit_data(data_byte);
	number_of_bytes_in_data = number_of_bytes_in_data - 1;
}*/

/* void Master_transmit_data_byte(unsigned char data_byte)
*  Skiftar en byte i register mellan master och slave. Väntar på att överföring blir klar.
*/
void Master_transmit_data_byte(unsigned char data_byte)
{
	SPDR = data_byte;
	while(!(SPSR & (1<<SPIF)))
	{
		;
	}
}

/* Send_address_to_slave(unsigned char address_byte)
*  Skickar adress-byte från master till sensor_slave
*/
int Send_address_to_sensor(unsigned char address_byte) 
{
	PORTB = (1<<PB4); //sätter SS låg
	Master_transmit_data_byte(address_byte); //Skickar adressbyten till sensor
	PORTB = (0<<PB4);
	return 0;
}

int main(void)
{
	COMMON_SET_PIN(PORTA, PORTA0);
	DDRA = 0x8A; //Sätter PA1, PA3 och PA7 till utgångar (för lamprona)
	DDRB = 0xB8; //Sätter SCK, MOSI, och SS till utgångar
	SPCR = 0xC0; //Aktiverar avbrott från SPI, aktiverar SPI, sätter modul till master. 
	SPSR = 0x01; //Sätter SCK till fosc/2
	
	Send_address_to_sensor(0x02);
	
    while(1)
    {
        //TODO:: Please write your application code 
    }
}
