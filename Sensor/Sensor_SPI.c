
#include <avr/io.h>
#include <avr/interrupt.h>
#include "../CommonLibrary/Common.h"
#include "Sensor_SPI.h"

static volatile uint8_t master_has_recieved_byte;
static volatile uint8_t has_recieved_give_ir_sensor_data;
static volatile uint8_t has_recieved_give_distance;
static volatile uint8_t has_recieved_start_calc_angle;
static volatile uint8_t has_recieved_give_angle;

static volatile uint8_t error;

void SPI_sensor_init(void)
{
	SPCR = 0xC0; //Aktiverar avbrott från SPI, aktiverar SPI, sätter modul till slave.
	DDRB = 0x41; //sätter MISO till  utgång och även PB0 till utgång, flagga SPI
	
	DDRD = 0x30; //utgångar på PD4 och PD5
	PORTD = 0; //test
	PORTD = 0x20; //test
	PORTD = 0; //test
	
	SPDR = 0x22;

	master_has_recieved_byte = 0; //initera variabler
	has_recieved_give_ir_sensor_data = 0;
	has_recieved_give_distance = 0;
	has_recieved_start_calc_angle = 0;
	has_recieved_give_angle = 0;
	
	error = 0;
}
//Avbrottsrutin SPI transmission complete
ISR(SPI_STC_vect)
{
	uint8_t byte_from_SPI = SPDR;
	switch (byte_from_SPI)
	{
		case MASTER_RECIEVED_BYTE:
			master_has_recieved_byte = 1;
			PORTD = 0;
			PORTD = 0x20;
			PORTD = 0;
			break;
		case ID_BYTE_GIVE_IR_SENSOR_DATA:
			has_recieved_give_ir_sensor_data = 1;
			break;
		case ID_BYTE_GIVE_DISTANCE:
			has_recieved_give_distance = 1;
			break;
		case ID_BYTE_START_CALC_ANGLE:
			has_recieved_start_calc_angle = 1;
			break;
		case ID_BYTE_GIVE_ANGLE:
			has_recieved_give_angle = 1;
			break;
		default:
			error = 1;
			PORTD = 0;
			PORTD = 0x20;
			PORTD = 0;
			break;
	}
}

uint8_t SPI_master_have_recieved_byte(void)
{
	uint8_t result = master_has_recieved_byte;
	master_has_recieved_byte = 0;
	return result;
}
uint8_t SPI_sensor_should_give_ir_sensor_data(void)
{
	uint8_t result = has_recieved_give_ir_sensor_data;
	has_recieved_give_ir_sensor_data = 0;
	return result;
}
uint8_t SPI_sensor_should_give_distance(void)
{
	uint8_t result = has_recieved_give_distance;
	has_recieved_give_distance = 0;
	return result;
}
uint8_t SPI_sensor_should_start_calc_angle(void) //byt namn angular_rate
{
	uint8_t result = has_recieved_start_calc_angle;
	has_recieved_start_calc_angle = 0;
	return result;
}
uint8_t SPI_sensor_should_give_angle(void) //ska bort
{
	uint8_t result = has_recieved_give_angle;
	has_recieved_give_angle = 0;
	return result;
}

/* void SPI_sensor_slave_send_id_byte(uint8_t id_byte)
*  Skickar id_byte till Master som framtvingar övriga bytes
*/
static void SPI_sensor_slave_send_id_byte(uint8_t id_byte)
{
	SPDR = id_byte;
	COMMON_TOGGLE_PIN(PORTB, PORTB0); // Hissa flagga
	while(!(SPI_master_have_recieved_byte())){}
}

/* void SPI_sensor_send(uint8_t id_byte, volatile uint8_t *data)
*  Skickar hela meddelanden från sensor till Master
*/
void SPI_sensor_send(uint8_t id_byte, uint8_t *data)
{
	uint8_t number_of_bytes_in_data = 0;
	switch (id_byte)
	{
		case ID_BYTE_IR_SENSOR_DATA:
		number_of_bytes_in_data = 6;
		break;
		case ID_BYTE_DISTANCE: 
		case ID_BYTE_ROTATION_FINISHED: // Fel! ska skrivas om!
		number_of_bytes_in_data = 1;
		break;
		default:
		error = 1;
		break;
	}
		
	cli();
	SPI_sensor_slave_send_id_byte(id_byte);
		
	while(number_of_bytes_in_data != 0)
	{
		if(SPSR & (1<<SPIF))
		{
			SPDR = data[(number_of_bytes_in_data - 1)];
			number_of_bytes_in_data--;
		}
	}
	sei();	
}