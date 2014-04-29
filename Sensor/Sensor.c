/*
 * Sensor.c
 *
 * Created: 2014-03-29 14:22:07
 *  Author: HuGu
 */ 


#include <avr/io.h>
#include <avr/interrupt.h>
#include "../CommonLibrary/Common.h"
#include "Sensor_SPI.h"
#include "Sensor_cm_converter.h"

volatile unsigned char reflex_count = 0;
volatile unsigned char test;
int baud;
volatile uint8_t sensor1, sensor2, sensor3, sensor4, sensor5;

volatile uint8_t ir_sensor_data[6];

volatile uint8_t count=0;

volatile unsigned char angular_value_rot;
volatile uint8_t angle;
volatile float angle_rot = 0;
volatile float angular_diff_rot;

void init_interrupts(){
		sei();  //set global interrupt
		ACSR = (1>>ACD)|(1>>ACBG)|(1<<ACIE)|(1>>ACIC)|(0>>ACIS0)|(0>>ACIS1);  //ACD=0 AC på, ACBG=0 external comparator pin AIN0, ACIE=1 interrupt enable, ACIC=0 Timer/Counter disabled, ACIS0..1=1 interrupt on rising output edge
		//DDRD = 1<<DDD6;
		PORTD = 1<<PORTD6;
		ADMUX = (1<<ADLAR)|(1<<REFS0)|(1<<MUX1)|(1<<MUX0); // Set the ADMUX
		ADCSRA = (1<<ADIE)|(1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0); //Set Prescaler to 128, Set ADCSRA to enable interrupts
		
		ADCSRA |= (1<<ADSC); //Start ADC
		//DIDR1 = (1<<AIN1D)|(1<<AIN0D); //
}

/*void USART_init(){
	DDRD = (1<<DDD2);  //Enable RFID
	UBRR1H = (unsigned char) (baud>>8);  //set baud rate
	UBRR1L = (unsigned char) baud;  //set baud rate
	UCSR1B = (1<<RXEN1);  //USRT1 recieve pin? Enable reciever
	UCSR1C = (1<<USBS1) | (3<<UCSZ10);  //Frame format, 8data, 2 stop bits
}*/

//ISR(INT1_vect){
	//test = PIND;
//}



ISR (ADC_vect)
{
	//count = 5;
	
	switch (count) 
	{
		
		case(0):
		ir_sensor_data[0] = S1_convert_sensor_value_left_front(ADCH);
		sensor1 = S1_convert_sensor_value_left_front(ADCH); //sensor1 får det AD-omvandlade värdet 
		count++; //adderar 1 till count
		ADMUX = (1<<ADLAR)|(1<<REFS0)|(1<<MUX2); //sätter ADMUX till PA4
		break;
		
		case(1):
		ir_sensor_data[1] = S2_convert_sensor_value__left_back(ADCH);
		sensor2 = S2_convert_sensor_value__left_back(ADCH); //sensor2 får det AD-omvandlade värdet 
		count++; //adderar 1 till count
		ADMUX = (1<<ADLAR)|(1<<REFS0)|(1<<MUX2)|(1<<MUX0); //Sätter ADMUX till PA5
		break;
		
		case(2):		
		sensor3 = S3_convert_sensor_value_right_front(ADCH); //sensor3 får det AD-omvandlade värdet 
		count++; //adderar 1 till count
		ADMUX = (1<<ADLAR)|(1<<REFS0)|(1<<MUX2)|(1<<MUX1); //Sätter ADMUX till PA6
		break;
		
		case(3):
		sensor4 = S4_convert_sensor_value_right_back(ADCH); //sensor4 får det AD-omvandlade värdet 
		count++; //adderar 1 till count
		ADMUX = (1<<ADLAR)|(1<<REFS0)|(1<<MUX2)|(1<<MUX1)|(1<<MUX0); //Sätter ADMUX till PA7
		break;
		
		case(4):		
		sensor5 = S5_convert_sensor_value_front_long(ADCH); //sensor5 får det AD-omvandlade värdet 
		count = 0; //nollställer count
		ADMUX = (1<<ADLAR)|(1<<REFS0)|(1<<MUX1)|(1<<MUX0); //Sätter ADMUX till PA3
		//angle = 90 - ((atan((sensor3-sensor4)/dist1)) + (atan((sensor2-sensor1)/dist2)))/2; //Ger vinkel från vänstra väggen 
		//diff_from_middle = 
		break;
		
		case(5):				
		if ((-20 < angle_rot) & (angle_rot < 20))
		{
			angular_value_rot = ADCH;
			angular_diff_rot = (angular_value_rot - ANGULAR_RATE_OFFSET) * ANGULAR_RATE_SENSITIVITY;
			angle_rot += angular_diff_rot/10000;
			ADMUX = (1<<ADLAR)|(1<<REFS0)|(1<<MUX1); // Sätter ADMUX till PA2
		} else 
		{  
			angle_rot = 0;
			//count = 5;
			reflex_count = 0;
			//send_angle(OK)
		}
		
		break;
	}
	
	ADCSRA |= (1<<ADSC); //startar omvandling
	
	
}

ISR(ANALOG_COMP_vect){
	if (PORTD & (1<<PORTD6)){
	PORTD &= 0xbf;
	}
	else{
	PORTD |= (1<<PORTD6);
	}
	reflex_count = reflex_count+1;
	ACSR |= (1<<ACI);//|(1>>ACD)|(1>>ACBG)|(1<<ACIE)|(1>>ACIC)|(0>>ACIS0)|(0>>ACIS1); 
}

int main(void)
{
	//,USART_init();
  	init_interrupts();
    while(1)
	{
	if(SPI_sensor_should_give_distance()) //skriv klart denna
	{
		//Skicka distans
		uint8_t distance[1];
		distance[0] = 0x00;
		SPI_sensor_send(ID_BYTE_DISTANCE, distance);
	}
	
	/*if {start_angle}
		{
			send_dist (reflex_count);
			ADMUX = (1<<ADLAR)|(1<<REFS0)|(1<<MUX1);
			count=5;
			
		} elseif {dist_req}
			{
				send_dist (reflex_count);
				reflex_count = 0;
			} 
			elseif {ir_req}
				{
			send_IR (vec(s1,s2,s3,s4,s5))
			send_angle (angle)
		         }
	
	USART_check //function Alt. av man aktiverar RFID här också
	ser ut som: Läs parallel in från usart och kolla om rätt ID
	om rätt, sätt en pinne kopplad till KOM avbrott
	*/
	}
}
			
			