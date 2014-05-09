
#ifndef FIREFLY_H_
#define FIREFLY_H_

void USART_init();
void USART_send_sensor_data_to_PC(uint8_t *data_ptr);
void USART_send_byte_to_PC(uint8_t id_byte, uint8_t sensor_value);
void USART_send_map_coordinates_to_PC(uint8_t id_byte, int8_t x_coordinate, int8_t y_coordinate);

#endif