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
double BASE_SPEED = 32000; // Halvfart, HÖGRE värde ger HÖGRE hastighet
double left_speed_factor = 0; // Mellan 0 och 2, HÖGRE värde ger HÖGRE hastighet
double right_speed_factor = 0; // Mellan 0 och 2, HÖGRE värde ger HÖGRE hastighet
	
//-----------------PORTDEFINITIONER----------------//
/* Portdefinitioner Motor
PORTD5: Höger styrka
PORTD4: Vänster styrka
PORTD3: Höger riktning
PORTD2: Vänster riktning
*/

//-----------------INITIERINGSFUNKTIONER----------------//
void Initialize_interrupt()
{	
	sei(); // Tillåter globala interrupt
	EIMSK |= (1<<INT2); // Tillåter avbrott 2
	EICRA |= (0<<ISC20) | (1<<ISC21); // Ger avbrott på låg flank
	
	MCUCR = 0x03;
	//GICR = 0x40; //External input, Från labb
}

void Initialize_pwm()
{
	// Motorer init
	PORTD = (1<< PORTD2) | (1<< PORTD3);
	ICR1 = COUNTER_MAX; // Räknarens tak
	OCR1A = BASE_SPEED * right_speed_factor; // Höger motor gräns
	OCR1B = BASE_SPEED * left_speed_factor; // Vänster motor gräns
	TCCR1A |= (1<< COM1A1) | (0<< COM1A0) | (1<< COM1B1) | (0<< COM1B0) | (1<< WGM11) | (0<< WGM10); // PWM-inställningar motor
	TCCR1B |= (1<< WGM13) | (1<< WGM12) | (0<< CS12) | (0<< CS11) | (1<< CS10); // PWM-inställningar motor
	
	//Gripklo init, kommer starta öppen
	OCR2A = 255; // Gripklo TOP
	OCR2B = 25; // Gripklon startar öppen
	TCCR2A |= (0<< WGM21) | (1<< WGM20) | (1<< COM2B1) | (0<< COM2B0); // PWM-Inställningar gripklo
	TCCR2B |= (1<< WGM22) | (1<< CS22) | (1<< CS21) | (0<< CS20); // PWM-inställningar gripklo
	
	DDRD |= (1<< DDD4) | (1<< DDD5) | (1<< DDD6); // Gör PD4.5.6 till utgångar för PWM ut	
}

//----------------MANUELLA KOMMANDOFUNKTIONER-----------//
// Stanna
void Stop()
{
	OCR1A = 0;
	OCR1B = 0;
}

// Åk Framåt
void Forward()
{	
	PORTD = (1<< PORTD2) | (1<< PORTD3);// Vänster - Höger
	left_speed_factor = 0.5; // Mellan 0 och 2
	right_speed_factor = 0.5; // Mellan 0 och 2
	OCR1A = BASE_SPEED * right_speed_factor; // Höger motor gräns
	OCR1B = BASE_SPEED * left_speed_factor; // Vänster motor gräns
}

// Åk Bakåt
void Back()
{
	PORTD = (0<< PORTD2) | (0<< PORTD3);
	OCR1A = BASE_SPEED * 0.5; // Uppdatera hastigheten
	OCR1B = BASE_SPEED * 0.5;
}

// Rotera Höger
void Rotate_right()
{
	PORTD = (1<<PORTD2) | (0<<PORTD3); // Vänster - Höger
	left_speed_factor = 1;
	right_speed_factor = 1;
	OCR1A = BASE_SPEED * right_speed_factor; // Uppdatera hastigheten
	OCR1B = BASE_SPEED * left_speed_factor;
}

// Rotera Vänster
void Rotate_left()
{
	PORTD = (0<<PORTD2) | (1<<PORTD3); // Vänster - Höger
	left_speed_factor = 1;
	right_speed_factor = 1;
	OCR1A = BASE_SPEED * right_speed_factor; // Uppdatera hastigheten
	OCR1B = BASE_SPEED * left_speed_factor;
}

// Sväng Höger
void Turn_right()
{
	PORTD = (1<<PORTD2) | (1<<PORTD3); // Vänster - Höger
	left_speed_factor = 1.5;
	right_speed_factor = 0.5; // Ska prövas fram, 1.8 så länge
	OCR1A = BASE_SPEED * right_speed_factor; // Uppdatera hastigheten
	OCR1B = BASE_SPEED * left_speed_factor;
}

// Sväng Vänster
void Turn_left()
{
	PORTD = (1<<PORTD2) | (1<<PORTD3); // Vänster - Höger
	left_speed_factor = 0.5;
	right_speed_factor = 1.5; // Ska prövas fram, 1.8 så länge
	OCR1A = BASE_SPEED * right_speed_factor; // Uppdatera hastigheten
	OCR1B = BASE_SPEED * left_speed_factor;
}

// Öppna Gripklo
void Open()
{
	OCR2B = 25;
}

// Stäng Gripklo
void Close()
{
	OCR2B = 20;
}

//----------------AUTONOMA REGLERFUNKTIONER-----------//
void Forward_regulated()
{
	// reglering efter sensorvärden
}

void Back_regulated()
{
	// reglering efter sensorvärden
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
	//full fart framåt:
	PORTD = (1<< PORTD2) | (1<< PORTD3);// Vänster - Höger
	left_speed_factor = 2; // Mellan 0 och 2
	right_speed_factor = 2; // Mellan 0 och 2
	OCR1A = BASE_SPEED * right_speed_factor; // Höger motor gräns
	OCR1B = BASE_SPEED * left_speed_factor; // Vänster motor gräns
	
	Delay_seconds(1);
	
	//medelfart
	PORTD = (1<< PORTD2) | (1<< PORTD3);// Vänster - Höger
	left_speed_factor = 1; // Mellan 0 och 2
	right_speed_factor = 1; // Mellan 0 och 2
	OCR1A = BASE_SPEED * right_speed_factor; // Höger motor gräns
	OCR1B = BASE_SPEED * left_speed_factor; // Vänster motor gräns
	
	Delay_seconds(2);
	
	//långsamt
	PORTD = (1<< PORTD2) | (1<< PORTD3);// Vänster - Höger
	left_speed_factor = 0.2; // Mellan 0 och 2
	right_speed_factor = 0.2; // Mellan 0 och 2
	OCR1A = BASE_SPEED * right_speed_factor; // Höger motor gräns
	OCR1B = BASE_SPEED * left_speed_factor; // Vänster motor gräns
	
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
ISR(INT2_vect) // TRYCKKNAPP på PB3
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
	
	// Dessa 3 rader är till för att kolla att RESET fungerar
	//Open();
	//Delay_seconds(1);
	//Close();
	
	// Använd avborttsknappen för att starta testprogram
	// Koden skrivs Testprogram()
	
	Loop();
	
	return 0;
}