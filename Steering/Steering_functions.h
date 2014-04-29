
#ifndef _STEERING_FUNCTIONS_H
#define _STEERING_FUNCTIONS_H

void Initialize_interrupt();
void Initialize_pwm();

void Stop();
void Forward();
void Backward();
void Rotate_right();
void Rotate_left();
void Turn_right();
void Turn_left();
void Open_claw();
void Close_claw();

void Forward_regulated(uint8_t, uint8_t);
void Backward_regulated();

void Delay_seconds(int seconds);
void Testprogram();
void gusfunk();
void Loop();


#endif
