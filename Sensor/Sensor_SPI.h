/*
 * Sensor_SPI.h 
 * Författare: mikiv293, linka950
 * 2014-06-05
 * version 1.0
 */ 

#ifndef _SENSOR_SPI_H
#define _SENSOR_SPI_H

void SPI_sensor_init(void);

uint8_t SPI_sensor_should_handle_number_of_reflex_counts_to_RFID(void);

void SPI_sensor_send_rotation_finished(void);
void SPI_sensor_send_reached_RFID(void);
void SPI_sensor_send_sensor_data(uint8_t *data);
void SPI_sensor_send_data_byte(uint8_t id_byte, uint8_t data_byte);

#endif