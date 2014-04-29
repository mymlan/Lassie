
#ifndef _SENSOR_SPI_H
#define _SENSOR_SPI_H

void SPI_sensor_init(void);

uint8_t SPI_master_have_recieved_byte(void);
uint8_t SPI_sensor_should_give_ir_sensor_data(void);
uint8_t SPI_sensor_should_give_distance(void);
uint8_t SPI_sensor_should_start_angular_rate_sensor(void);

void SPI_sensor_send_rotation_finished(void);
void SPI_sensor_send(uint8_t id_byte,uint8_t *data);

#endif