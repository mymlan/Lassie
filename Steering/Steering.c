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
	double BASE_SPEED; // Halvfart, högre värde ger lägre hastighet
	double left_speed_factor; // Mellan 0 och 2
	double right_speed_factor; // Mellan 0 och 2
	
//-----------------FUNKTIONSDEFINITIONER----------------//
/* Portdefinitioner Motor
PORTD5: Höger styrka
PORTD4: Vänster styrka
PORTD3: Höger riktning
PORTD2: Vänster riktning
*/
//Stanna
void Stop()
{
	left_speed_factor = 2;
	right_speed_factor = 2;
	OCR1A = BASE_SPEED * left_speed_factor; // Uppdatera hastigheten 
	OCR1B = BASE_SPEED * right_speed_factor;  
}


//Åk Framåt
void Forward()
{	
	PORTD |= (1<<PORTD2) | (1<<PORTD3);// Vänster - Höger
	left_speed_factor = 0; // Sätts efter reglering senare
	right_speed_factor = 0;
	OCR1A = BASE_SPEED * left_speed_factor; // Uppdatera hastigheten
	OCR1B = BASE_SPEED * right_speed_factor;
}

//Åk Bakåt
void Back()
{
	PORTD |= (0<<PORTD2) | (0<<PORTD3);// Vänster  - Höger
	left_speed_factor = 0; // Sätts efter reglering senare
	right_speed_factor = 0;
	OCR1A = BASE_SPEED * left_speed_factor; // Uppdatera hastigheten
	OCR1B = BASE_SPEED * right_speed_factor;
}

//Rotera Höger
void Rotate_right()
{
	PORTD |= (1<<PORTD2) | (0<<PORTD3); // Vänster - Höger
	left_speed_factor = 0;
	right_speed_factor = 0;
	OCR1A = BASE_SPEED * left_speed_factor; // Uppdatera hastigheten
	OCR1B = BASE_SPEED * right_speed_factor;
}

//Rotera Vänster
void Rotate_left()
{
	PORTD |= (0<<PORTD2) | (1<<PORTD3); // Vänster - Höger
	left_speed_factor = 0;
	right_speed_factor = 0;
	OCR1A = BASE_SPEED * left_speed_factor; // Uppdatera hastigheten
	OCR1B = BASE_SPEED * right_speed_factor;
}

//Sväng Höger
void Turn_right()
{
	PORTD |= (1<<PORTD2) | (1<<PORTD3); // Vänster - Höger
	left_speed_factor = 0;
	right_speed_factor = 1.8; // Ska prövas fram, 1.8 så länge
	OCR1A = BASE_SPEED * left_speed_factor; // Uppdatera hastigheten
	OCR1B = BASE_SPEED * right_speed_factor;
}

//Sväng Vänster
void Turn_left()
{
	PORTD |= (1<<PORTD2) | (1<<PORTD3); // Vänster - Höger
	left_speed_factor = 1.8;
	right_speed_factor = 0; // Ska prövas fram, 1.8 så länge
	OCR1A = BASE_SPEED * left_speed_factor; // Uppdatera hastigheten
	OCR1B = BASE_SPEED * right_speed_factor;
}

//Öppna Gripklo
void Open()
{
	OCR2B = 25;
}

//Stäng Gripklo
void Close()
{
	OCR2B = 20;
}


//-------------AVBROTTSRUTIN---------------------//

ISR(INT1_vect) // TRYCKKNAPP på PB3
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

	MCUCR = 0x07; //External input, Från labb
	
	//Motor init, Kommer starta stillaståendes
	long int COUNTER_MAX = 65535;
	double BASE_SPEED = 32768; // Halvfart, högre värde ger lägre hastighet
	double left_speed_factor = 2; // Mellan 0 och 2
	double right_speed_factor = 2; // Mellan 0 och 2
	PORTD |= (1<<PORTD3) | (1<<PORTD4); // Motorernas riktning
	ICR1 = COUNTER_MAX; // Räknarens tak
	OCR1A = BASE_SPEED * left_speed_factor; // Höger motor gräns
	OCR1B = BASE_SPEED * right_speed_factor; // Vänster motor gräns
	TCCR1A |= (1<< COM1A1) | (1<< COM1A0) | (1<< COM1B0) | (1<< COM1B1) | (1<< WGM11) | (0<< WGM10); // Nödvändiga grejor för PWM - motor
	TCCR1B |= (1<< WGM13) | (1<< WGM12) | (0<< CS12) | (0<< CS11) | (1<< CS10); // Nödvändiga grejor för PWM - motor

	//Gripklo init, kommer starta öppen
	OCR2A = 255; // Gripklo TOP
	OCR2B = 25; // Gripklon startar öppen
	TCCR2A |= (0<< WGM21) | (1<< WGM20) | (1<< COM2B1) | (0<< COM2B0);//PWM-Inställningar Gripklo
	TCCR2B |= (1<< WGM22) | (1<< CS22) | (1<< CS21) | (0<< CS20); //PWM-inställningar Gripklo
	DDRD |= (1<< DDD4) | (1<< DDD5) | (1<< DDD6); //  Gör PD4.5.6 till utgångar för PWM ut
	
	
	while (1)
	{
	}
	
	return 0;
}