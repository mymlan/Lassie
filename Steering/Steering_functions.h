
#ifndef _STEERING_FUNCTIONS_H
#define _STEERING_FUNCTIONS_H

void Steering_initialize_7_segment_display_error_pins();
void Steering_initialize_pwm();

void Stop();
void Forward();
void Backward();
void Rotate_right();
void Rotate_left();
void Turn_right();
void Turn_left();
void Tight_turn_right();
void Tight_turn_left();
void Open_claw();
void Close_claw();

void Forward_regulated(uint8_t, uint8_t);
void Backward_regulated(uint8_t, uint8_t);

#endif