
#ifndef FIREFLY_H_
#define FIREFLY_H_

void USART_init();
void USART_send_sensor_data_to_PC(uint8_t *data_ptr);
void USART_send_byte_to_PC(uint8_t id_byte, uint8_t sensor_value);
void USART_send_map_coordinates_to_PC(uint8_t id_byte, int8_t x_coordinate, int8_t y_coordinate);
void USART_send_map_parameters_to_PC(uint8_t robot_direction, uint8_t size_of_all_nodes, uint8_t following_path, uint8_t enable_node_editing, uint8_t level, uint8_t x_coordinate, uint8_t y_coordinates);

#endif