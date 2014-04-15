
#ifndef _SENSOR_SPI_H
#define _SENSOR_SPI_H

void SPI_sensor_init(void);

uint8_t SPI_master_have_recieved_byte(void);
uint8_t SPI_should_give_sensor_data(void);
uint8_t SPI_should_give_distance(void);
uint8_t SPI_should_start_calc_angle(void);
uint8_t SPI_should_give_angle(void);

void SPI_slave_send_byte(uint8_t byte);

void SPI_sensor_send(uint8_t id_byte,volatile uint8_t *data);

#endif