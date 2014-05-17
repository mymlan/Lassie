#ifndef _SENSOR_CM_CONVERTER_H
#define _SENSOR_CM_CONVERTER_H

void init_interrupts();
void USART_init();
void init_variable();
void init_button_calibrate_angular_sensor();

uint8_t S1_convert_sensor_value_left_front(uint8_t digital_distance);
uint8_t S2_convert_sensor_value_left_back(uint8_t digital_distance);
uint8_t S3_convert_sensor_value_right_front(uint8_t digital_distance);
uint8_t S4_convert_sensor_value_right_back(uint8_t digital_distance);
uint8_t S5_convert_sensor_value_front_long(uint8_t digital_distance);
uint8_t calculate_angle_corridor(uint8_t left_front, uint8_t left_back, uint8_t right_front, uint8_t  right_back);
//uint8_t calculate_diff_from_middle_corridor(int8_t angle_in_corridor, uint8_t left_front, uint8_t left_back, uint8_t right_front, uint8_t right_back);
uint8_t calculate_diff_from_middle_corridor(uint8_t left_front, uint8_t left_back, uint8_t right_front, uint8_t right_back);

uint8_t ir_sensor_data[7]; // volatile fungerar inte med send funktionen
volatile uint16_t reflex_count;
volatile uint8_t next_sensor_to_be_converted;

#endif