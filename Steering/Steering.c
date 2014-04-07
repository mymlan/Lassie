/*
* Steering.c
*
* Created: 3/31/2014 2:07:31 PM
* Author: henba136
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU 1000000UL
#include <util/delay.h>

//-------------VARIABLER/KONSTANTER---------------//

long int COUNTER_MAX = 65535;
double BASE_SPEED = 32000; // Halvfart, H�GRE v�rde ger H�GRE hastighet
double left_speed_factor = 0; // Mellan 0 och 2, H�GRE v�rde ger H�GRE hastighet
double right_speed_factor = 0; // Mellan 0 och 2, H�GRE v�rde ger H�GRE hastighet
	
//-----------------PORTDEFINITIONER----------------//
/* Portdefinitioner Motor
PORTD5: H�ger styrka
PORTD4: V�nster styrka
PORTD3: H�ger riktning
PORTD2: V�nster riktning
*/

//-----------------INITIERINGSFUNKTIONER----------------//
void Initialize_interrupt()
{	
	sei(); // Till�ter globala interrupt
	EIMSK |= (1<<INT2); // Till�ter avbrott 2
	EICRA |= (0<<ISC20) | (1<<ISC21); // Ger avbrott p� l�g flank
	
	MCUCR = 0x03;
	//GICR = 0x40; //External input, Fr�n labb
}

void Initialize_pwm()
{
	// Motorer init
	PORTD = (1<< PORTD2) | (1<< PORTD3);
	ICR1 = COUNTER_MAX; // R�knarens tak
	OCR1A = BASE_SPEED * right_speed_factor; // H�ger motor gr�ns
	OCR1B = BASE_SPEED * left_speed_factor; // V�nster motor gr�ns
	TCCR1A |= (1<< COM1A1) | (0<< COM1A0) | (1<< COM1B1) | (0<< COM1B0) | (1<< WGM11) | (0<< WGM10); // PWM-inst�llningar motor
	TCCR1B |= (1<< WGM13) | (1<< WGM12) | (0<< CS12) | (0<< CS11) | (1<< CS10); // PWM-inst�llningar motor
	
	//Gripklo init, kommer starta �ppen
	OCR2A = 255; // Gripklo TOP
	OCR2B = 25; // Gripklon startar �ppen
	TCCR2A |= (0<< WGM21) | (1<< WGM20) | (1<< COM2B1) | (0<< COM2B0); // PWM-Inst�llningar gripklo
	TCCR2B |= (1<< WGM22) | (1<< CS22) | (1<< CS21) | (0<< CS20); // PWM-inst�llningar gripklo
	
	DDRD |= (1<< DDD4) | (1<< DDD5) | (1<< DDD6); // G�r PD4.5.6 till utg�ngar f�r PWM ut	
}

//----------------MANUELLA KOMMANDOFUNKTIONER-----------//
// Stanna
void Stop()
{
	OCR1A = 0;
	OCR1B = 0;
}

// �k Fram�t
void Forward()
{	
	PORTD = (1<< PORTD2) | (1<< PORTD3);// V�nster - H�ger
	left_speed_factor = 0.5; // Mellan 0 och 2
	right_speed_factor = 0.5; // Mellan 0 och 2
	OCR1A = BASE_SPEED * right_speed_factor; // H�ger motor gr�ns
	OCR1B = BASE_SPEED * left_speed_factor; // V�nster motor gr�ns
}

// �k Bak�t
void Back()
{
	PORTD = (0<< PORTD2) | (0<< PORTD3);
	OCR1A = BASE_SPEED * 0.5; // Uppdatera hastigheten
	OCR1B = BASE_SPEED * 0.5;
}

// Rotera H�ger
void Rotate_right()
{
	PORTD = (1<<PORTD2) | (0<<PORTD3); // V�nster - H�ger
	left_speed_factor = 1;
	right_speed_factor = 1;
	OCR1A = BASE_SPEED * right_speed_factor; // Uppdatera hastigheten
	OCR1B = BASE_SPEED * left_speed_factor;
}

// Rotera V�nster
void Rotate_left()
{
	PORTD = (0<<PORTD2) | (1<<PORTD3); // V�nster - H�ger
	left_speed_factor = 1;
	right_speed_factor = 1;
	OCR1A = BASE_SPEED * right_speed_factor; // Uppdatera hastigheten
	OCR1B = BASE_SPEED * left_speed_factor;
}

// Sv�ng H�ger
void Turn_right()
{
	PORTD = (1<<PORTD2) | (1<<PORTD3); // V�nster - H�ger
	left_speed_factor = 1.5;
	right_speed_factor = 0.5; // Ska pr�vas fram, 1.8 s� l�nge
	OCR1A = BASE_SPEED * right_speed_factor; // Uppdatera hastigheten
	OCR1B = BASE_SPEED * left_speed_factor;
}

// Sv�ng V�nster
void Turn_left()
{
	PORTD = (1<<PORTD2) | (1<<PORTD3); // V�nster - H�ger
	left_speed_factor = 0.5;
	right_speed_factor = 1.5; // Ska pr�vas fram, 1.8 s� l�nge
	OCR1A = BASE_SPEED * right_speed_factor; // Uppdatera hastigheten
	OCR1B = BASE_SPEED * left_speed_factor;
}

// �ppna Gripklo
void Open()
{
	OCR2B = 25;
}

// St�ng Gripklo
void Close()
{
	OCR2B = 20;
}

//----------------AUTONOMA REGLERFUNKTIONER-----------//
void Forward_regulated()
{
	// reglering efter sensorv�rden
}

void Back_regulated()
{
	// reglering efter sensorv�rden
}

//-----------------DIVERSE FUNKTIONER----------------//
// Pausning / Delay
void Delay_seconds(int seconds)
{
	for (int i = 0; i < seconds; i++)
	{
		for (int j = 0; j < 10000; j++)
		{
			_delay_ms(1);
		}
	}
}

// Test
void Testprogram()
{
	Forward();
	Delay_seconds(1);
	
	Back();
	Delay_seconds(1);
	
	Rotate_left();
	Delay_seconds(1);
	
	Rotate_right();
	Delay_seconds(1);
	
	Turn_left();
	Delay_seconds(1);
	
	Turn_right();
	Delay_seconds(1);
	
	Stop();
}

//gustavs testfunktion
void gusfunk()
{
	//full fart fram�t:
	PORTD = (1<< PORTD2) | (1<< PORTD3);// V�nster - H�ger
	left_speed_factor = 2; // Mellan 0 och 2
	right_speed_factor = 2; // Mellan 0 och 2
	OCR1A = BASE_SPEED * right_speed_factor; // H�ger motor gr�ns
	OCR1B = BASE_SPEED * left_speed_factor; // V�nster motor gr�ns
	
	Delay_seconds(1);
	
	//medelfart
	PORTD = (1<< PORTD2) | (1<< PORTD3);// V�nster - H�ger
	left_speed_factor = 1; // Mellan 0 och 2
	right_speed_factor = 1; // Mellan 0 och 2
	OCR1A = BASE_SPEED * right_speed_factor; // H�ger motor gr�ns
	OCR1B = BASE_SPEED * left_speed_factor; // V�nster motor gr�ns
	
	Delay_seconds(2);
	
	//l�ngsamt
	PORTD = (1<< PORTD2) | (1<< PORTD3);// V�nster - H�ger
	left_speed_factor = 0.2; // Mellan 0 och 2
	right_speed_factor = 0.2; // Mellan 0 och 2
	OCR1A = BASE_SPEED * right_speed_factor; // H�ger motor gr�ns
	OCR1B = BASE_SPEED * left_speed_factor; // V�nster motor gr�ns
	
	Delay_seconds(4);
	Stop();
	
}

// Tom loop
void Loop()
{
	while (1)
	{}
}

//-------------AVBROTTSRUTIN---------------------//
ISR(INT2_vect) // TRYCKKNAPP p� PB3
{
	//TODO: en case(styrbeslut)

	int styrbeslut = 123;
	switch(styrbeslut)
	{
		case 0: Stop();
		break;
		case 1: Forward();
		break;
		case 2: Back();
		break;
		case 3: Rotate_right();
		break;
		case 4: Rotate_left();
		break;
		case 5: Turn_right();
		break;
		case 6: Turn_left();
		break;
		case 7: Open();
		break;
		case 8: Close();
		break;
		case 42: Testprogram();
		break;
		case 123: gusfunk();
		break;
		default: Stop();
		break;
	}
}

//----------------MAIN--------------//
int main(void)
{
	// Initiering
	Initialize_interrupt();
	Initialize_pwm();
	
	// Dessa 3 rader �r till f�r att kolla att RESET fungerar
	//Open();
	//Delay_seconds(1);
	//Close();
	
	// Anv�nd avborttsknappen f�r att starta testprogram
	// Koden skrivs Testprogram()
	
	Loop();
	
	return 0;
}