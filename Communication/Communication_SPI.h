
#ifndef _COMMUNICATION_SPI_H
#define _COMMUNICATION_SPI_H

static void SPI_Master_init();

static uint8_t SPI_Master_recieve_data_byte_from_sensor();
// funk som anropas av avbrottsvektorn då sensor har ngt att skicka.
static void SPI_Master_recieve_sensor_data();
static void SPI_Master_recieve_distance();
static void SPI_Master_recieve_angle();

static void SPI_Master_transmit_data_byte(uint8_t data_byte);
void SPI_Master_send_to_sensor(uint8_t address_byte);

void SPI_Master_send_to_steering(uint8_t id_byte, volatile uint8_t *data_ptr);

#endif