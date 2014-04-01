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
	
	//VIKTIGT: PD6 f�r ej ges h�g signal (kan f�rst�ra gripklon)
	/* Portdefinitioner
	PORTD5: H�ger styrka
	PORTD4: V�nster styrka
	PORTD3: H�ger riktning
	PORTD2: V�nster riktning
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

	ICR1 = COUNTER_MAX; // R�knarens tak
	OCR1A = BASE_SPEED * LEFT_SPEED_FACTOR; // H�ger motor gr�ns
	OCR1B = BASE_SPEED * RIGHT_SPEED_FACTOR; // V�nster motor gr�ns
	
	//OCR2A = 70;//70-90 bra siffror; // Gripklo gr�nd
	// H�gt OCR1x ger l�ngsammare hastighet
	
	TCCR1A |= (1<< COM1A1) | (1<< COM1A0) | (1<< COM1B0) | (1<< COM1B1) | (1<< WGM11) | (0<< WGM10); // N�dv�ndiga grejor f�r PWM - motor
	TCCR1B |= (1<< WGM13) | (1<< WGM12) | (0<< CS12) | (0<< CS11) | (1<< CS10); // N�dv�ndiga grejor f�r PWM - motor
	
	/*ICR3 = 20000; // R�knarens tak
	OCR3A = 5000;
	OCR3B = 10000; // V�nster motor gr�ns
	
	TCCR3A |= (1<< COM3A1) | (1<< COM3A0) | (1<< COM3B1) | (1<< COM3B0) | (1<< WGM31) | (0<< WGM30); // Gripklo
	TCCR3B |= (1<< WGM33) | (1<< WGM32) | (0<< CS32) | (0<< CS31) | (1<< CS30); // Gripklo	
	*/
	//TCCR2A |= (1<< WGM21) | (1<< WGM20) | (1<< COM2B1) | (0<< COM2B0);//COM2B 11 el 10 f�rrikting
	//TCCR2B |= (1<< WGM22) | (1<< CS22) | (0<< CS21) | (1<< CS20);
	DDRD |= (1<< DDD4) | (1<< DDD5) | (1<< DDD6); //  G�r PD4.5.6 till utg�ngar f�r PWM ut
	//DDRB |= (1<< DDB6) | (1<< DDB7); // Pinne ut till gripklo
	// Ett problem som finns �r att gripklon griper ihop hela tiden..
	

	/*
	// Litet sv�ngningsprogramm
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
			TCCR2A |= (1<< COM2B1) | (1<< COM2B0);//COM2B 11 el 10 f�r rikting
			OCR2A = 10;	
			for(int x = 0; x<10000; x++)
			{
				_delay_ms(1);
			}
			OCR2A = 70;
			TCCR2A |= (1<< COM2B1) | (0<< COM2B0);//COM2B 11 el 10 f�r rikting
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