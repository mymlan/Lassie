
#ifndef _SENSOR_SPI_H
#define _SENSOR_SPI_H

void SPI_sensor_init(void);

uint8_t SPI_master_have_recieved_byte(void);
uint8_t SPI_sensor_should_give_ir_sensor_data(void);
uint8_t SPI_sensor_should_give_distance(void);
uint8_t SPI_sensor_should_start_calc_angle(void);
uint8_t SPI_sensor_should_give_angle(void);

void SPI_sensor_send(uint8_t id_byte,uint8_t *data);

#endif