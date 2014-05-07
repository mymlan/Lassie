
#ifndef _COMMUNICATION_SPI_H
#define _COMMUNICATION_SPI_H

void SPI_Master_init(void);

void SPI_Master_send_id_byte_to_sensor(uint8_t id_byte);
void SPI_Master_send_sensor_data_to_steering(uint8_t *data_ptr);
void SPI_Master_send_command_to_steering(uint8_t id_byte, uint8_t command);

uint8_t SPI_map_should_handle_new_sensor_data(void);
uint8_t SPI_map_should_handle_new_distance(void);
uint8_t SPI_map_should_handle_rotation_finished(void);
uint8_t SPI_map_should_handle_rfid(void);

#endif