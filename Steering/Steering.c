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
	double BASE_SPEED = 32000; // Halvfart, h�gre v�rde ger l�gre hastighet
	double left_speed_factor = 2; // Mellan 0 och 2
	double right_speed_factor = 2; // Mellan 0 och 2
	
//-----------------FUNKTIONSDEFINITIONER----------------//
/* Portdefinitioner Motor
PORTD5: H�ger styrka
PORTD4: V�nster styrka
PORTD3: H�ger riktning
PORTD2: V�nster riktning
*/
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

//Stanna
void Stop() // Fungerar
{
	OCR1A = COUNTER_MAX;
	OCR1B = COUNTER_MAX;
}

//�k Fram�t
void Forward()
{	
	PORTD = (1<< PORTD2) | (1<< PORTD3);// V�nster - H�ger
	left_speed_factor = 1.5; // Mellan 0 och 2
	right_speed_factor = 1.5; // Mellan 0 och 2
	OCR1A = BASE_SPEED * right_speed_factor; // H�ger motor gr�ns
	OCR1B = BASE_SPEED * left_speed_factor; // V�nster motor gr�ns
}

//�k Bak�t
void Back()
{
	PORTD = (0<< PORTD2) | (0<< PORTD3);
	OCR1A = BASE_SPEED * 1.6; // Uppdatera hastigheten
	OCR1B = BASE_SPEED * 1.6;
}

//Rotera H�ger
void Rotate_right()
{
	PORTD = (1<<PORTD2) | (0<<PORTD3); // V�nster - H�ger
	left_speed_factor = 1.5;
	right_speed_factor = 1.5;
	OCR1A = BASE_SPEED * right_speed_factor; // Uppdatera hastigheten
	OCR1B = BASE_SPEED * left_speed_factor;
}

//Rotera V�nster
void Rotate_left()
{
	PORTD = (0<<PORTD2) | (1<<PORTD3); // V�nster - H�ger
	left_speed_factor = 1.5;
	right_speed_factor = 1.5;
	OCR1A = BASE_SPEED * right_speed_factor; // Uppdatera hastigheten
	OCR1B = BASE_SPEED * left_speed_factor;
}

//Sv�ng H�ger
void Turn_right()
{
	PORTD = (1<<PORTD2) | (1<<PORTD3); // V�nster - H�ger
	left_speed_factor = 1;
	right_speed_factor = 1.5; // Ska pr�vas fram, 1.8 s� l�nge*/
	OCR1A = BASE_SPEED * right_speed_factor; // Uppdatera hastigheten
	OCR1B = BASE_SPEED * left_speed_factor;
}

//Sv�ng V�nster
void Turn_left()
{
	PORTD = (1<<PORTD2) | (1<<PORTD3); // V�nster - H�ger
	left_speed_factor = 1.5;
	right_speed_factor = 1; // Ska pr�vas fram, 1.8 s� l�nge
	OCR1A = BASE_SPEED * right_speed_factor; // Uppdatera hastigheten
	OCR1B = BASE_SPEED * left_speed_factor;
}

//�ppna Gripklo
void Open()
{
	OCR2B = 25;
}

//St�ng Gripklo
void Close()
{
	OCR2B = 20;
}

//Test
void Test()
{
	Rotate_left();
	Delay_seconds(2);
	
	Rotate_right();
	Delay_seconds(2);
	
	Open();
	Delay_seconds(2);
	
	Close();
	Delay_seconds(2);
	
	Stop();
}

//-------------AVBROTTSRUTIN---------------------//

ISR(INT2_vect) // TRYCKKNAPP p� PB3
{
	//TODO: en case(styrbeslut)

	int styrbeslut = 42;
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
		case 42: Test();
		break;
		default: Stop();
		break;
	}
}

//----------------MAIN--------------//
int main(void)
{
	double left_speed_factor = 2; // Mellan 0 och 2
	double right_speed_factor = 2; // Mellan 0 och 2

	EIMSK |= (1<<INT2); //till�ter avbrott1
	EICRA |= (0<<ISC20) | (1<<ISC21); //Ger avbrott p� l�g flank
	
	MCUCR = 0x03;
	//GICR = 0x40; //External input, Fr�n labb
	
	//Motor init, Kommer starta stillast�endes
	//COUNTER_MAX = 65535;
	//BASE_SPEED = 32768; // Halvfart, h�gre v�rde ger l�gre hastighet
	//left_speed_factor = 2; // Mellan 0 och 2
	//right_speed_factor = 2; // Mellan 0 och 2
	//PORTD |= (1<< PORTD2) | (1<< PORTD3); // Motorernas riktning
	//DDRD = (1<< DDD2) | (1<< DDD3);
	PORTD = (1<< PORTD2) | (1<< PORTD3);
	ICR1 = COUNTER_MAX; // R�knarens tak
	OCR1A = BASE_SPEED * right_speed_factor; // H�ger motor gr�ns
	OCR1B = BASE_SPEED * left_speed_factor; // V�nster motor gr�ns
	TCCR1A |= (1<< COM1A1) | (1<< COM1A0) | (1<< COM1B0) | (1<< COM1B1) | (1<< WGM11) | (0<< WGM10); // N�dv�ndiga grejor f�r PWM - motor
	TCCR1B |= (1<< WGM13) | (1<< WGM12) | (0<< CS12) | (0<< CS11) | (1<< CS10); // N�dv�ndiga grejor f�r PWM - motor

	//Gripklo init, kommer starta �ppen
	OCR2A = 255; // Gripklo TOP
	OCR2B = 25; // Gripklon startar �ppen
	TCCR2A |= (0<< WGM21) | (1<< WGM20) | (1<< COM2B1) | (0<< COM2B0);//PWM-Inst�llningar Gripklo
	TCCR2B |= (1<< WGM22) | (1<< CS22) | (1<< CS21) | (0<< CS20); //PWM-inst�llningar Gripklo
	DDRD |= (1<< DDD4) | (1<< DDD5) | (1<< DDD6); //  G�r PD4.5.6 till utg�ngar f�r PWM ut
	
	
	// Testrogram
	Open();
	Forward();
	Delay_seconds(1); // 1 sekund delay
	Close();
	Stop();
	Delay_seconds(1); // 1 sekund delay

	Open();
	Back();
	Delay_seconds(1); // 1 sekund delay
	Close();
	Stop();
	Delay_seconds(1); // 1 sekund delay
	
	Open();
	Rotate_left();
	Delay_seconds(1); // 1 sekund delay
	Close();
	Stop();
	Delay_seconds(1); // 1 sekund delay

	Open();
	Rotate_right();
	Delay_seconds(1); // 1 sekund delay
	Close();
	Stop();
	Delay_seconds(1); // 1 sekund delay
	
	Open();
	Turn_left();
	Delay_seconds(1); // 1 sekund delay
	Close();
	Stop();
	Delay_seconds(1); // 1 sekund delay

	Open();
	Turn_right();
	Delay_seconds(1); // 1 sekund delay
	Close();
	Stop();
	Delay_seconds(1); // 1 sekund delay
	
	while (1)
	{
	}
	
	return 0;
}