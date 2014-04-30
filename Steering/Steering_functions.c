
//#define F_CPU 16000000UL har ändrat klockan till en som går i 18432000Hz
#define F_CPU 18432000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "Steering_functions.h"

//-------------VARIABLER/KONSTANTER---------------//

long int COUNTER_MAX = 65535;
double BASE_SPEED = 40000; // Halvfart, HÖGRE värde ger HÖGRE hastighet
double left_speed_factor = 0; // Mellan 0 och 2, HÖGRE värde ger HÖGRE hastighet
double right_speed_factor = 0; // Mellan 0 och 2, HÖGRE värde ger HÖGRE hastighet
double K_P = 0.01; // Proportionella konstanten
double K_D = 0.01; // Deriveringskonstanten

//-----------------PORTDEFINITIONER----------------//
/* Portdefinitioner Motor
PORTD5: Höger styrka PWM
PORTD4: Vänster styrka PWM
PORTD3: Höger riktning
PORTD2: Vänster riktning
*/

//-----------------INITIERINGSFUNKTIONER----------------//
void Initialize_interrupt()
{
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
void Backward()
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
void Open_claw()
{
	OCR2B = 25;
}

// Stäng Gripklo
void Close_claw()
{
	OCR2B = 20;
}

//----------------AUTONOMA REGLERFUNKTIONER-----------//
void Forward_regulated(uint8_t regulator_error, uint8_t regulator_angle)//arg: uint8_t regulator_error, uint8_t regulator_angle
{
	// reglering efter sensorvärden
	//double regulator_error = 20; // Felet e i mm (exempelvis)
	//double regulator_angle = 0; // Vinkeln theta i rad
	PORTD = (1<< PORTD2) | (1<< PORTD3); // Vänster - Höger riktning
	double adjusted_speed = K_P * regulator_error + K_D * tan(regulator_angle);
	OCR1A = BASE_SPEED * (1 - adjusted_speed); // Höger motor gräns
	OCR1B = BASE_SPEED * (1 + adjusted_speed); // Vänster motor gräns
	// Det kan bli fel om adjusted_speed blir för stor (beror av BASE_SPEED). När vi vet BASE_SPEED får vi lägga in ett tak på adjusted_speed.
}

void Backward_regulated(uint8_t regulator_error, uint8_t regulator_angle)
{
	// reglering efter sensorvärden
	PORTD = (0<< PORTD2) | (0<< PORTD3); // Vänster - Höger riktning
	double adjusted_speed = K_P * regulator_error + K_D * tan(regulator_angle);
	OCR1A = BASE_SPEED * (1 - adjusted_speed); // Höger motor gräns
	OCR1B = BASE_SPEED * (1 + adjusted_speed); // Vänster motor gräns
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
	Forward_regulated(0, 0);
	Delay_seconds(2);
	
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

	int styrbeslut = 42;
	switch(styrbeslut)
	{
		case 0: Stop();
		break;
		case 1: Forward();
		break;
		case 2: Backward();
		break;
		case 3: Rotate_right();
		break;
		case 4: Rotate_left();
		break;
		case 5: Turn_right();
		break;
		case 6: Turn_left();
		break;
		case 7: Open_claw();
		break;
		case 8: Close_claw();
		break;
		case 42: Testprogram();
		break;
		case 123: gusfunk();
		break;
		default: Stop();
		break;
	}
}