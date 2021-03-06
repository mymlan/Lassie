/*
 * Sensor_cm_converter.h
 * Författare: hugjo456, gusno119
 * 2014-06-05
 * version 1.0
 */ 

#ifndef _SENSOR_CM_CONVERTER_H
#define _SENSOR_CM_CONVERTER_H

uint8_t ir_sensor_data[7];
volatile uint16_t reflex_count;
volatile uint8_t number_of_reflex_counts_to_RFID;
volatile uint8_t count_down_number_of_reflex_counts_to_RFID_requested;

volatile uint8_t next_sensor_to_be_converted;

void Sensor_init_AD_converter();
void Sensor_init_analog_comparator();
void Sensor_init_USART_for_RFID();
void Sensor_init_button_for_calibrating_angular_rate_sensor();

void Calibrate_angular_rate_sensor();

uint8_t Calculate_approx_angle_corridor(uint8_t left_front, uint8_t left_back, uint8_t right_front, uint8_t  right_back);
uint8_t Calculate_distance_from_right_wall(uint8_t left_front, uint8_t left_back, uint8_t right_front, uint8_t right_back);

uint8_t S1_convert_sensor_value_left_front(uint8_t digital_distance);
uint8_t S2_convert_sensor_value_left_back(uint8_t digital_distance);
uint8_t S3_convert_sensor_value_right_front(uint8_t digital_distance);
uint8_t S4_convert_sensor_value_right_back(uint8_t digital_distance);
uint8_t S5_convert_sensor_value_front_long(uint8_t digital_distance);

#endif