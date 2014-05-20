#define F_CPU (18432000L)

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "../CommonLibrary/Common.h"
#include "Sensor_SPI.h"
#include "Sensor_cm_converter.h"

//----------------VARIABLER/KONSTANTER---------------//
volatile uint8_t REFLEX_COUNT_DISTANCE_PER_COLOUR_FIELD = 49;
volatile uint8_t sensor_has_recieved_number_of_reflex_counts_to_RFID;

//----------------INITIERINGSFUNKTION----------------//
void SPI_sensor_init(void)
{
	SPCR = 0xC0; //Aktiverar avbrott från SPI, aktiverar SPI, sätter modul till slave.
	DDRB = 0x41; //sätter MISO till  utgång och även PB0 till utgång, flagga SPI
	
	sensor_has_recieved_number_of_reflex_counts_to_RFID = 0;
}

//-----------------STATIC FUNKTIONER----------------//

/* void SPI_sensor_slave_send_id_byte(uint8_t id_byte)
*  Skickar id_byte till Master som framtvingar övriga bytes
*/
static void SPI_sensor_slave_send_id_byte(uint8_t id_byte)
{
	SPDR = id_byte;
	COMMON_TOGGLE_PIN(PORTB, PORTB0); // Hissa flagga
	while(!(SPSR & (1<<SPIF))){}
}

static uint8_t SPI_sensor_recieve_byte(void)
{
	uint8_t number_of_bytes_in_data = 1;
	while(number_of_bytes_in_data != 0)
	{
		if (SPSR & (1<<SPIF))
		{
			number_of_bytes_in_data--;
		}
	}
	return SPDR;
}

//----------------AVBROTTSVEKTORER----------------//
//Avbrottsrutin SPI transmission complete
ISR(SPI_STC_vect)
{
	uint8_t byte_from_SPI = SPDR;
	switch (byte_from_SPI)
	{
		case ID_BYTE_GIVE_IR_SENSOR_DATA:
			SPI_sensor_send_sensor_data(ir_sensor_data);
			break;
		case ID_BYTE_GIVE_DISTANCE:
		{
			uint8_t distance = ((reflex_count*REFLEX_COUNT_DISTANCE_PER_COLOUR_FIELD) / 10);
			SPI_sensor_send_data_byte(ID_BYTE_DISTANCE, distance);
			reflex_count = 0;
			break;
		}
		case ID_BYTE_START_ANGULAR_RATE_SENSOR:
			//ACSR |= (1<<ACD);  //Stänger av Analog Comparator (reflexsensor) // BUGG i påsättningen senare!
			ADMUX = (1<<ADLAR)|(1<<REFS0)|(1<<MUX1);
			next_sensor_to_be_converted = ANGULAR_RATE;
			break;
		case ID_BYTE_COUNT_DOWN_REFLEX_SENSOR:
			number_of_reflex_counts_to_RFID = SPI_sensor_recieve_byte();
			sensor_has_recieved_number_of_reflex_counts_to_RFID = 1;
			break;
		default:
			break;
	}
}

//--------------FRÅGE-FUNKTIONER-----------------//
uint8_t SPI_sensor_should_handle_number_of_reflex_counts_to_RFID(void)
{
	uint8_t result = sensor_has_recieved_number_of_reflex_counts_to_RFID;
	sensor_has_recieved_number_of_reflex_counts_to_RFID = 0;
	return result;
}

//-------------SPI FUNKTIONER SENSOR----------------//
void SPI_sensor_send_rotation_finished(void)
{
	SPI_sensor_slave_send_id_byte(ID_BYTE_ROTATION_FINISHED);
}

void SPI_sensor_send_reached_RFID(void)
{
	SPI_sensor_slave_send_id_byte(ID_BYTE_REACHED_RFID);
}

void SPI_sensor_send_sensor_data(uint8_t *data)
{
	uint8_t number_of_bytes_in_data = NUMBER_OF_BYTES_IR_SENSOR_DATA;	
	SPI_sensor_slave_send_id_byte(ID_BYTE_IR_SENSOR_DATA);
		
	while(number_of_bytes_in_data != 0)
	{	
		if(SPSR & (1<<SPIF))
		{
			SPDR = data[(number_of_bytes_in_data - 1)];
			number_of_bytes_in_data--;
		}
	}
}

void SPI_sensor_send_data_byte(uint8_t id_byte, uint8_t data_byte)
{
	SPI_sensor_slave_send_id_byte(id_byte);
	if(SPSR & (1<<SPIF))
	{
		SPDR = data_byte;
	}
}