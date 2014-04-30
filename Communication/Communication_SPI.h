
#ifndef _COMMUNICATION_SPI_H
#define _COMMUNICATION_SPI_H

void SPI_Master_init(void);

void SPI_Master_send_to_sensor(uint8_t address_byte);
void SPI_Master_send_to_steering(uint8_t id_byte, uint8_t *data_ptr);
void SPI_Master_send_command_to_steering(uint8_t id_byte, uint8_t command);

#endif