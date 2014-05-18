
#define F_CPU 18432000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "Steering_functions.h"
#include "../CommonLibrary/Common.h"

//-------------VARIABLER/KONSTANTER---------------//

uint16_t COUNTER_MAX = 65535;
uint16_t BASE_SPEED = 30000; // Halvfart, HÖGRE värde ger HÖGRE hastighet, 40000 halva

float K_P = 0.004; // Proportionella konstanten, 0.003 är bra
float K_D = 1.5; // Deriveringskonstanten, 1.5 är bra
float adjusted_speed;

static uint16_t pwm_right_wheel_fast_speed = 45000;
//static uint16_t pwm_right_wheel_normal_speed = 30000;
static uint16_t pwm_right_wheel_slow_speed = 15000;
static uint16_t pwm_right_wheel_stop = 0;

static uint16_t pwm_left_wheel_fast_speed = 45000;
static uint16_t pwm_left_wheel_normal_speed = 30000;
static uint16_t pwm_left_wheel_slow_speed = 15000;
static uint16_t pwm_left_wheel_stop = 0;

static uint16_t pwm_claw_closed = 20;
static uint16_t pwm_claw_open = 25;

uint16_t adjusted_speed_KP_right_wheel;
uint16_t adjusted_speed_KP_left_wheel;
uint16_t adjusted_speed_KD_right_wheel;
uint16_t adjusted_speed_KD_left_wheel;

//-----------------PORTDEFINITIONER----------------//
/* Portdefinitioner Motor
PORTD5: Höger styrka PWM
PORTD4: Vänster styrka PWM
PORTD3: Höger riktning
PORTD2: Vänster riktning

OCR1A = Höger PWM;
OCR1B = Vänster PWM;
*/

static void Set_wheels_both_goes_forward()
{
	PORTD = (1<< PORTD2) | (1<< PORTD3);
}
static void Set_wheels_both_goes_backward()
{
	PORTD = (0<< PORTD2) | (0<< PORTD3);
}
static void Set_wheels_left_forward_right_backward()
{
	PORTD = (1<<PORTD2) | (0<<PORTD3);
}
static void Set_wheels_left_backward_right_forward()
{
	PORTD = (0<<PORTD2) | (1<<PORTD3);
}

//-----------------INITIERINGSFUNKTION----------------//
void Initialize_error_pins()
{
	DDRA = 0xFF; // Alla port A pinnar ut.
	PORTA = 0x00; // Inget error = 0
}

void Initialize_pwm()
{
	// Motorer init
	Set_wheels_both_goes_forward();
	ICR1 = COUNTER_MAX; // Räknarens tak
	OCR1A = pwm_right_wheel_stop;
	OCR1B = pwm_left_wheel_stop;
	TCCR1A |= (1<< COM1A1) | (0<< COM1A0) | (1<< COM1B1) | (0<< COM1B0) | (1<< WGM11) | (0<< WGM10); // PWM-inställningar motor
	TCCR1B |= (1<< WGM13) | (1<< WGM12) | (0<< CS12) | (0<< CS11) | (1<< CS10); // PWM-inställningar motor
	
	//Gripklo init, kommer starta öppen
	OCR2A = 255; // Gripklo TOP
	OCR2B = pwm_claw_open; // Gripklon startar öppen
	TCCR2A |= (0<< WGM21) | (1<< WGM20) | (1<< COM2B1) | (0<< COM2B0); // PWM-Inställningar gripklo
	TCCR2B |= (1<< WGM22) | (1<< CS22) | (1<< CS21) | (0<< CS20); // PWM-inställningar gripklo
	
	DDRD |= (1<< DDD4) | (1<< DDD5) | (1<< DDD6); // Gör PD4.5.6 till utgångar för PWM ut
}

//----------------MANUELLA KOMMANDOFUNKTIONER-----------//
void Stop()
{
	OCR1A = pwm_right_wheel_stop;
	OCR1B = pwm_left_wheel_stop;
}
void Forward()
{
	Set_wheels_both_goes_forward();
	OCR1A = pwm_right_wheel_fast_speed;
	OCR1B = pwm_left_wheel_fast_speed;
}
void Backward()
{
	Set_wheels_both_goes_backward();
	OCR1A = pwm_right_wheel_fast_speed;
	OCR1B = pwm_left_wheel_fast_speed;
}
void Rotate_right()
{
	Set_wheels_left_forward_right_backward();
	OCR1A = pwm_right_wheel_fast_speed;
	OCR1B = pwm_left_wheel_fast_speed;
}
void Rotate_left()
{
	Set_wheels_left_backward_right_forward();
	OCR1A = pwm_right_wheel_fast_speed;
	OCR1B = pwm_left_wheel_fast_speed;
}
void Turn_right()
{
	Set_wheels_both_goes_forward();
	OCR1A = pwm_right_wheel_slow_speed;
	OCR1B = pwm_left_wheel_fast_speed;
}
void Turn_left()
{
	Set_wheels_both_goes_forward();
	OCR1A = pwm_right_wheel_fast_speed;
	OCR1B = pwm_left_wheel_slow_speed;
}
void Tight_turn_right()
{
	Set_wheels_left_forward_right_backward();
	OCR1A = pwm_right_wheel_slow_speed; // OBS slow här för höger sensorer detekterar korsning tidigare
	OCR1B = pwm_left_wheel_fast_speed;
}
void Tight_turn_left()
{
	Set_wheels_left_backward_right_forward();
	OCR1A = pwm_right_wheel_fast_speed;
	OCR1B = pwm_left_wheel_normal_speed;
}
void Open_claw()
{
	OCR2B = pwm_claw_open;
}
void Close_claw()
{
	OCR2B = pwm_claw_closed;
}

//----------------AUTONOMA REGLERFUNKTIONER-----------//


void Forward_regulated(uint8_t regulator_angle, uint8_t regulator_error)
{
	Set_wheels_both_goes_forward();
	
	/*adjusted_speed = K_P * (regulator_error - 100) + K_D * tan(( - regulator_angle + 90) * 3.1415 / 180.0f);
	if (adjusted_speed >= 0.3)
	{
		adjusted_speed = 0.3;
	}
	else if (adjusted_speed <= -0.3)
	{
		
		adjusted_speed = -0.3;
	}
	OCR1A = BASE_SPEED * (1 - adjusted_speed); // Höger motor gräns
	OCR1B = BASE_SPEED * (1 + adjusted_speed); // Vänster motor gräns*/
	
	Lookup_table_K_D(regulator_angle);
	Lookup_table_K_P(regulator_error);
	OCR1A = adjusted_speed_KD_right_wheel + adjusted_speed_KP_right_wheel - BASE_SPEED;
	OCR1B = adjusted_speed_KD_left_wheel + adjusted_speed_KP_left_wheel - BASE_SPEED;
}

void Backward_regulated(uint8_t regulator_angle, uint8_t regulator_error)
{
	Set_wheels_both_goes_backward();
	/*adjusted_speed = K_P * (regulator_error) + K_D * tan(regulator_angle - 90);
	OCR1A = BASE_SPEED * (1 + adjusted_speed); // Höger motor gräns
	OCR1B = BASE_SPEED * (1 - adjusted_speed); // Vänster motor gräns*/
	Lookup_table_K_D(regulator_angle);
	Lookup_table_K_P(regulator_error);
	OCR1A = adjusted_speed_KD_right_wheel + adjusted_speed_KP_right_wheel - BASE_SPEED;
	OCR1B = adjusted_speed_KD_left_wheel + adjusted_speed_KP_left_wheel - BASE_SPEED;
}

void Lookup_table_K_P(uint8_t regulator_error)
{
	if(regulator_error >= 142)
	{
		adjusted_speed_KP_right_wheel = 23400;
		adjusted_speed_KP_left_wheel = 36600;
	}
	else if(regulator_error < 142 && regulator_error >= 132)
	{
		adjusted_speed_KP_right_wheel = 25560;
		adjusted_speed_KP_left_wheel = 34440;
	}
	else if(regulator_error < 132 && regulator_error >= 123)
	{
		adjusted_speed_KP_right_wheel = 26700;
		adjusted_speed_KP_left_wheel = 33300;
	}
	else if(regulator_error < 123 && regulator_error >= 115)
	{
		adjusted_speed_KP_right_wheel = 27720;
		adjusted_speed_KP_left_wheel = 32280;
	}
	else if(regulator_error < 115 && regulator_error >= 108)
	{
		adjusted_speed_KP_right_wheel = 28740;
		adjusted_speed_KP_left_wheel = 31260;
	}
	else if(regulator_error < 108 && regulator_error >= 102)
	{
		adjusted_speed_KP_right_wheel = 29520;
		adjusted_speed_KP_left_wheel = 30480;
	}
	else if(regulator_error < 102 && regulator_error > 98)
	{
		adjusted_speed_KP_right_wheel = 30000;
		adjusted_speed_KP_left_wheel = 30000;
	}
	else if(regulator_error <= 98 && regulator_error > 92)
	{
		adjusted_speed_KP_right_wheel = 30480;
		adjusted_speed_KP_left_wheel = 29520;
	}
	else if(regulator_error <= 92 && regulator_error > 85)
	{
		adjusted_speed_KP_right_wheel = 31260;
		adjusted_speed_KP_left_wheel = 28740;
	}
	else if(regulator_error <= 85 && regulator_error > 77)
	{
		adjusted_speed_KP_right_wheel = 32280;
		adjusted_speed_KP_left_wheel = 27720;
	}
	else if(regulator_error <= 77 && regulator_error > 68)
	{
		adjusted_speed_KP_right_wheel = 33330;
		adjusted_speed_KP_left_wheel = 26700;
	}
	else if(regulator_error <= 68 && regulator_error > 58)
	{
		adjusted_speed_KP_right_wheel = 34440;
		adjusted_speed_KP_left_wheel = 25560;
	}
	else if(regulator_error <= 58)
	{
		adjusted_speed_KP_right_wheel = 36600;
		adjusted_speed_KP_left_wheel = 22400;
	}
}

void Lookup_table_K_D(uint8_t regulator_angle)
{
	if(regulator_angle >= 116)
	{
		adjusted_speed_KD_right_wheel = 20435;
		adjusted_speed_KD_left_wheel = 39565;
	}
	else if(regulator_angle < 116 && regulator_angle >= 111)
	{
		adjusted_speed_KD_right_wheel = 22066;
		adjusted_speed_KD_left_wheel = 37934;
	}
	else if(regulator_angle < 111 && regulator_angle >= 108)
	{
		adjusted_speed_KD_right_wheel = 24076;
		adjusted_speed_KD_left_wheel = 35924;
	}
	else if(regulator_angle < 108 && regulator_angle >= 105)
	{
		adjusted_speed_KD_right_wheel = 26063;
		adjusted_speed_KD_left_wheel = 33937;
	}
	else if(regulator_angle < 105 && regulator_angle >= 102)
	{
		adjusted_speed_KD_right_wheel = 28035;
		adjusted_speed_KD_left_wheel = 31965;
	}
	else if(regulator_angle < 102 && regulator_angle > 98)
	{
		adjusted_speed_KD_right_wheel = 30000;
		adjusted_speed_KD_left_wheel = 30000;
	}
	else if(regulator_angle <= 98 && regulator_angle > 95)
	{
		adjusted_speed_KD_right_wheel = 31965;
		adjusted_speed_KD_left_wheel = 28035;
	}
	else if(regulator_angle <= 95 && regulator_angle > 92)
	{
		adjusted_speed_KD_right_wheel = 33937;
		adjusted_speed_KD_left_wheel = 26063;
	}
	else if(regulator_angle <= 92 && regulator_angle > 89)
	{
		adjusted_speed_KD_right_wheel = 35924;
		adjusted_speed_KD_left_wheel = 24076;
	}
	else if(regulator_angle <= 89 && regulator_angle > 84)
	{
		adjusted_speed_KD_right_wheel = 37934;
		adjusted_speed_KD_left_wheel = 22066;
	}
	else if(regulator_angle <= 84)
	{
		adjusted_speed_KD_right_wheel = 39565;
		adjusted_speed_KD_left_wheel = 20435;
	}
}