﻿
#ifndef FIREFLY_H_
#define FIREFLY_H_

void USART_init();
void USART_send_sensor_data_to_PC(uint8_t id_byte, uint8_t *data_ptr);
void USART_send_one_sensor_value_to_PC(uint8_t id_byte, uint8_t sensor_value);

#endif /* FIREFLY_H_ */