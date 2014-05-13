
#ifndef _COMMUNICATION_SPI_H
#define _COMMUNICATION_SPI_H

uint8_t communication_sensor_data[7];
volatile uint8_t communication_distance;
volatile uint8_t rfid_id;

void SPI_Master_init(void);

uint8_t SPI_map_should_handle_new_sensor_data(void);
uint8_t SPI_map_should_handle_new_distance(void);
uint8_t SPI_map_should_handle_rotation_finished(void);
uint8_t SPI_map_should_handle_rfid(void);

void SPI_Master_send_id_byte_to_sensor(uint8_t id_byte);
void SPI_map_send_id_byte_to_sensor(uint8_t id_byte);

void SPI_Master_send_sensor_data_to_steering(uint8_t *data_ptr);

void SPI_Master_send_command_to_steering(uint8_t id_byte, uint8_t command);
void SPI_map_send_command_to_steering(uint8_t id_byte, uint8_t command);

#endif