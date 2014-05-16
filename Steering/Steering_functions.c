
#define F_CPU 18432000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "Steering_functions.h"
#include "../CommonLibrary/Common.h"

//-------------VARIABLER/KONSTANTER---------------//

uint16_t COUNTER_MAX = 65535;
uint16_t BASE_SPEED = 30000; // Halvfart, HÖGRE värde ger HÖGRE hastighet, 40000 halva

double K_P = 0.004; // Proportionella konstanten, 0.003 är bra
double K_D = 1.5; // Deriveringskonstanten, 1.5 är bra
double adjusted_speed;

static uint16_t pwm_right_wheel_fast_speed = 45000;
static uint16_t pwm_right_wheel_normal_speed = 30000;
static uint16_t pwm_right_wheel_slow_speed = 15000;
static uint16_t pwm_right_wheel_stop = 0;

static uint16_t pwm_left_wheel_fast_speed = 45000;
static uint16_t pwm_left_wheel_normal_speed = 30000;
static uint16_t pwm_left_wheel_slow_speed = 15000;
static uint16_t pwm_left_wheel_stop = 0;

static uint16_t pwm_claw_closed = 20;
static uint16_t pwm_claw_open = 25;

uint16_t adjusted_speed_right_wheel;
uint16_t adjusted_speed_left_wheel;

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
	OCR1A = pwm_right_wheel_normal_speed;
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
	
	adjusted_speed = K_P * (regulator_error - 100) + K_D * tan(( - regulator_angle + 90) * 3.1415 / 180.0f);
	if (adjusted_speed >= 0.3)
	{
		adjusted_speed = 0.3;
	}
	else if (adjusted_speed <= -0.3)
	{
		adjusted_speed = -0.3;
	}
	OCR1A = BASE_SPEED * (1 - adjusted_speed); // Höger motor gräns
	OCR1B = BASE_SPEED * (1 + adjusted_speed); // Vänster motor gräns
	
	//Lookup_table_K_D(regulator_angle);
	//nästa lookup
}

void Backward_regulated(uint8_t regulator_angle, uint8_t regulator_error)
{
	Set_wheels_both_goes_backward();
	adjusted_speed = K_P * (regulator_error) + K_D * tan(regulator_angle - 90);
	OCR1A = BASE_SPEED * (1 + adjusted_speed); // Höger motor gräns
	OCR1B = BASE_SPEED * (1 - adjusted_speed); // Vänster motor gräns
}

void Lookup_table_K_D(uint8_t regulator_angle)
{
	if(regulator_angle >= 116)
	{
		OCR1A = 20435;
		OCR1B = 39565;
	}
	else if(regulator_angle < 116 && regulator_angle >= 111)
	{
		OCR1A = 22066;
		OCR1B = 37934;
	}
	else if(regulator_angle < 111 && regulator_angle >= 108)
	{
		OCR1A = 24076;
		OCR1B = 35924;
	}
	else if(regulator_angle < 108 && regulator_angle >= 105)
	{
		OCR1A = 26063;
		OCR1B = 33937;
	}
	else if(regulator_angle < 105 && regulator_angle >= 102)
	{
		OCR1A = 28035;
		OCR1B = 31965;
	}
	else if(regulator_angle < 102 && regulator_angle > 98)
	{
		OCR1A = 30000;
		OCR1B = 30000;
	}
	else if(regulator_angle <= 98 && regulator_angle > 95)
	{
		OCR1A = 31965;
		OCR1B = 28035;
	}
	else if(regulator_angle <= 95 && regulator_angle > 92)
	{
		OCR1A = 33937;
		OCR1B = 26063;
	}
	else if(regulator_angle <= 92 && regulator_angle > 89)
	{
		OCR1A = 35924;
		OCR1B = 24076;
	}
	else if(regulator_angle <= 89 && regulator_angle > 84)
	{
		OCR1A = 37934;
		OCR1B = 22066;
	}
	else if(regulator_angle <= 84)
	{
		OCR1A = 39565;
		OCR1B = 20435;
	}
}