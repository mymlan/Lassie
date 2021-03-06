/*
 * Communication_SPI.h
 * Författare: mikiv293, linka950
 * 2014-06-05
 * version 1.0
 */

#ifndef _COMMUNICATION_SPI_H
#define _COMMUNICATION_SPI_H

uint8_t communication_sensor_data[7];
volatile uint8_t communication_distance;
volatile uint8_t rfid_id;

void SPI_Master_init(void);

uint8_t SPI_map_should_handle_new_sensor_data(void);
uint8_t SPI_map_should_handle_new_distance(void);
uint8_t SPI_map_should_handle_rotation_finished(void);
uint8_t SPI_map_should_handle_RFID(void);
uint8_t SPI_map_should_handle_reached_RFID(void);

void SPI_Master_send_id_byte_to_sensor(uint8_t id_byte);
void SPI_map_send_id_byte_to_sensor(uint8_t id_byte);

void SPI_map_send_number_of_reflex_count_to_RFID_to_sensor(uint8_t number_of_reflex_count);

void SPI_Master_send_command_to_steering(uint8_t id_byte, uint8_t command);
void SPI_map_send_command_to_steering(uint8_t id_byte, uint8_t command);

#endif