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

ISR(ADC_vect) // GLOBAL
{
	PORTB = ADCH;
}
ISR(INT0_vect) // TRYCKKNAPP
{
	ADCSRA = 0xCB;
}

int main(void)
{

	// External input
	MCUCR = 0x07;
	// Global interrupt
	sei();
	// Loop
	
	//VIKTIGT: PD6 får ej ges hög signal (kan förstöra gripklon)
	/* Portdefinitioner
	PORTD5: Höger styrka
	PORTD4: Vänster styrka
	PORTD3: Höger riktning
	PORTD2: Vänster riktning
	*/
	
	//---------PWM-test------------------//
	
/*
1. Starta motor
2. vanta tid1
3. stang av motor
4. vanta tid2
reapeat

pwm = tid1 /(tid1+tid2)	
*/	
	long int COUNTER_MAX = 65535;
	double BASE_SPEED = 32768; // Halvfart
	double LEFT_SPEED_FACTOR = 1; // Mellan 0 och 2
	double RIGHT_SPEED_FACTOR = 2; // Mellan 0 och 2

	ICR1 = COUNTER_MAX; // Räknarens tak
	OCR1A = BASE_SPEED * LEFT_SPEED_FACTOR; // Höger motor gräns
	OCR1B = BASE_SPEED * RIGHT_SPEED_FACTOR; // Vänster motor gräns
	
	//OCR2A = 70;//70-90 bra siffror; // Gripklo gränd
	// Högt OCR1x ger långsammare hastighet
	
	TCCR1A |= (1<< COM1A1) | (1<< COM1A0) | (1<< COM1B0) | (1<< COM1B1) | (1<< WGM11) | (0<< WGM10); // Nödvändiga grejor för PWM - motor
	TCCR1B |= (1<< WGM13) | (1<< WGM12) | (0<< CS12) | (0<< CS11) | (1<< CS10); // Nödvändiga grejor för PWM - motor
	
	/*ICR3 = 20000; // Räknarens tak
	OCR3A = 5000;
	OCR3B = 10000; // Vänster motor gräns
	
	TCCR3A |= (1<< COM3A1) | (1<< COM3A0) | (1<< COM3B1) | (1<< COM3B0) | (1<< WGM31) | (0<< WGM30); // Gripklo
	TCCR3B |= (1<< WGM33) | (1<< WGM32) | (0<< CS32) | (0<< CS31) | (1<< CS30); // Gripklo	
	*/
	//TCCR2A |= (1<< WGM21) | (1<< WGM20) | (1<< COM2B1) | (0<< COM2B0);//COM2B 11 el 10 förrikting
	//TCCR2B |= (1<< WGM22) | (1<< CS22) | (0<< CS21) | (1<< CS20);
	DDRD |= (1<< DDD4) | (1<< DDD5) | (1<< DDD6); //  Gör PD4.5.6 till utgångar för PWM ut
	//DDRB |= (1<< DDB6) | (1<< DDB7); // Pinne ut till gripklo
	// Ett problem som finns är att gripklon griper ihop hela tiden..
	

	/*
	// Litet svängningsprogramm
	OCR1B = 0;
	_delay_ms(12800);
	OCR1B = COUNTER_MAX;
	_delay_ms(12800);
	OCR1A = 0;
	_delay_ms(12800);
	OCR1A = COUNTER_MAX;
	_delay_ms(12800);
	OCR1A = BASE_SPEED*RIGHT_SPEED_FACTOR;
	OCR1B = BASE_SPEED*LEFT_SPEED_FACTOR;
	_delay_ms(12800);
	OCR1B = COUNTER_MAX;
	_delay_ms(12800);
	OCR1A = 0;
	_delay_ms(12800);
	OCR1A = COUNTER_MAX;
	*/
	while (1)
	{
/*			for(int x = 0; x<10000; x++)
			{
				_delay_ms(1);
			}
			TCCR2A |= (1<< COM2B1) | (1<< COM2B0);//COM2B 11 el 10 för rikting
			OCR2A = 10;	
			for(int x = 0; x<10000; x++)
			{
				_delay_ms(1);
			}
			OCR2A = 70;
			TCCR2A |= (1<< COM2B1) | (0<< COM2B0);//COM2B 11 el 10 för rikting
	*/			
	/*TCCR2A |= (1<< COM2B1) | (0<< COM2B1);
	OCR2A = 70;
	_delay_ms(100);
		
	TCCR2A |= (1<< COM2B1) | (1<< COM2B0);
	OCR2A = 50;
	_delay_ms(1000);
	*/
	}
	
	return 0;
}