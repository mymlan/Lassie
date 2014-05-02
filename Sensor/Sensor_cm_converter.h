
#ifndef _SENSOR_CM_CONVERTER_H
#define _SENSOR_CM_CONVERTER_H

void init_interrupts();

uint8_t S1_convert_sensor_value_left_front(uint8_t digital_distance);
uint8_t S2_convert_sensor_value__left_back(uint8_t digital_distance);
uint8_t S3_convert_sensor_value_right_front(uint8_t digital_distance);
uint8_t S4_convert_sensor_value_right_back(uint8_t digital_distance);
uint8_t S5_convert_sensor_value_front_long(uint8_t digital_distance);

uint8_t ir_sensor_data[7]; // volatile fungerar inte med send funktionen

#endif