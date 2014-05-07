
#ifndef _SENSOR_SPI_H
#define _SENSOR_SPI_H

void SPI_sensor_init(void);

void SPI_sensor_send_rotation_finished(void);
void SPI_sensor_send_sensor_data(uint8_t *data);
void SPI_sensor_send_data_byte(uint8_t id_byte, uint8_t data_byte);

#endif