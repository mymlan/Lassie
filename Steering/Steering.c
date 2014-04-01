/*
* Steering.c
*
* Created: 3/31/2014 2:07:31 PM
* Author: henba136
*/

#include <avr/io.h>
#include <avr/interrupt.h>


//-------------VARIABLER/KONSTANTER---------------//

	long int COUNTER_MAX;
	double BASE_SPEED; // Halvfart, h�gre v�rde ger l�gre hastighet
	double left_speed_factor; // Mellan 0 och 2
	double right_speed_factor; // Mellan 0 och 2
	
//-----------------FUNKTIONSDEFINITIONER----------------//
/* Portdefinitioner Motor
PORTD5: H�ger styrka
PORTD4: V�nster styrka
PORTD3: H�ger riktning
PORTD2: V�nster riktning
*/
//Stanna
void Stop()
{
	left_speed_factor = 2;
	right_speed_factor = 2;
	OCR1A = BASE_SPEED * left_speed_factor; // Uppdatera hastigheten 
	OCR1B = BASE_SPEED * right_speed_factor;  
}


//�k Fram�t
void Forward()
{	
	PORTD |= (1<<PORTD2) | (1<<PORTD3);// V�nster - H�ger
	left_speed_factor = 0; // S�tts efter reglering senare
	right_speed_factor = 0;
	OCR1A = BASE_SPEED * left_speed_factor; // Uppdatera hastigheten
	OCR1B = BASE_SPEED * right_speed_factor;
}

//�k Bak�t
void Back()
{
	PORTD |= (0<<PORTD2) | (0<<PORTD3);// V�nster  - H�ger
	left_speed_factor = 0; // S�tts efter reglering senare
	right_speed_factor = 0;
	OCR1A = BASE_SPEED * left_speed_factor; // Uppdatera hastigheten
	OCR1B = BASE_SPEED * right_speed_factor;
}

//Rotera H�ger
void Rotate_right()
{
	PORTD |= (1<<PORTD2) | (0<<PORTD3); // V�nster - H�ger
	left_speed_factor = 0;
	right_speed_factor = 0;
	OCR1A = BASE_SPEED * left_speed_factor; // Uppdatera hastigheten
	OCR1B = BASE_SPEED * right_speed_factor;
}

//Rotera V�nster
void Rotate_left()
{
	PORTD |= (0<<PORTD2) | (1<<PORTD3); // V�nster - H�ger
	left_speed_factor = 0;
	right_speed_factor = 0;
	OCR1A = BASE_SPEED * left_speed_factor; // Uppdatera hastigheten
	OCR1B = BASE_SPEED * right_speed_factor;
}

//Sv�ng H�ger
void Turn_right()
{
	PORTD |= (1<<PORTD2) | (1<<PORTD3); // V�nster - H�ger
	left_speed_factor = 0;
	right_speed_factor = 1.8; // Ska pr�vas fram, 1.8 s� l�nge
	OCR1A = BASE_SPEED * left_speed_factor; // Uppdatera hastigheten
	OCR1B = BASE_SPEED * right_speed_factor;
}

//Sv�ng V�nster
void Turn_left()
{
	PORTD |= (1<<PORTD2) | (1<<PORTD3); // V�nster - H�ger
	left_speed_factor = 1.8;
	right_speed_factor = 0; // Ska pr�vas fram, 1.8 s� l�nge
	OCR1A = BASE_SPEED * left_speed_factor; // Uppdatera hastigheten
	OCR1B = BASE_SPEED * right_speed_factor;
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


//-------------AVBROTTSRUTIN---------------------//

ISR(INT1_vect) // TRYCKKNAPP p� PB3
{
	//TODO: en case(styrbeslut)

	int styrbeslut = 1;
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
		default: Stop();
		break;
	}
}

//----------------MAIN--------------//
int main(void)
{

	MCUCR = 0x07; //External input, Fr�n labb
	
	//Motor init, Kommer starta stillast�endes
	long int COUNTER_MAX = 65535;
	double BASE_SPEED = 32768; // Halvfart, h�gre v�rde ger l�gre hastighet
	double left_speed_factor = 2; // Mellan 0 och 2
	double right_speed_factor = 2; // Mellan 0 och 2
	PORTD |= (1<<PORTD3) | (1<<PORTD4); // Motorernas riktning
	ICR1 = COUNTER_MAX; // R�knarens tak
	OCR1A = BASE_SPEED * left_speed_factor; // H�ger motor gr�ns
	OCR1B = BASE_SPEED * right_speed_factor; // V�nster motor gr�ns
	TCCR1A |= (1<< COM1A1) | (1<< COM1A0) | (1<< COM1B0) | (1<< COM1B1) | (1<< WGM11) | (0<< WGM10); // N�dv�ndiga grejor f�r PWM - motor
	TCCR1B |= (1<< WGM13) | (1<< WGM12) | (0<< CS12) | (0<< CS11) | (1<< CS10); // N�dv�ndiga grejor f�r PWM - motor

	//Gripklo init, kommer starta �ppen
	OCR2A = 255; // Gripklo TOP
	OCR2B = 25; // Gripklon startar �ppen
	TCCR2A |= (0<< WGM21) | (1<< WGM20) | (1<< COM2B1) | (0<< COM2B0);//PWM-Inst�llningar Gripklo
	TCCR2B |= (1<< WGM22) | (1<< CS22) | (1<< CS21) | (0<< CS20); //PWM-inst�llningar Gripklo
	DDRD |= (1<< DDD4) | (1<< DDD5) | (1<< DDD6); //  G�r PD4.5.6 till utg�ngar f�r PWM ut
	
	
	while (1)
	{
	}
	
	return 0;
}