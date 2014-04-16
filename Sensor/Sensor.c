/*
 * Sensor.c
 *
 * Created: 2014-03-29 14:22:07
 *  Author: HuGu
 */ 


#include <avr/io.h>
#include <avr/interrupt.h>
volatile unsigned char reflex_count = 0;
volatile unsigned char test;
int baud;
volatile unsigned char sensor1, sensor2, sensor3, sensor4, sensor5;

volatile uint8_t count=0;

volatile unsigned char sensitivity = 1.28;
volatile unsigned char offset = 134;
volatile unsigned char angular_value;
volatile float angle = 0;
volatile float angular_diff;

void init_interrupts(){
		sei();  //set global interrupt
		ACSR = (1>>ACD)|(1>>ACBG)|(1<<ACIE)|(1>>ACIC)|(0>>ACIS0)|(0>>ACIS1);  //ACD=0 AC på, ACBG=0 external comparator pin AIN0, ACIE=1 interrupt enable, ACIC=0 Timer/Counter disabled, ACIS0..1=1 interrupt on rising output edge
		//DDRD = 1<<DDD6;
		PORTD = 1<<PORTD6;
		ADMUX = (1<<ADLAR)|(1<<REFS0)|(1<<MUX1)|(1<<MUX0); // Set the ADMUX
		ADCSRA = (1<<ADIE)|(1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0); //Set Prescaler to 128, Set ADCSRA to enable interrupts
		
		ADCSRA |= (1<<ADSC); //Start ADC
		//DIDR1 = (1<<AIN1D)|(1<<AIN0D);
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

float S1_sensor_value_front(float n)
{
	float cm_value;
	
	if (n <= 146 && n >= 105)
	{
		cm_value = ((n-228)/-20.5);
	}
	else if (n <=105 && n >= 93)
	{
		cm_value = ((n - 186)/-13.5);
	}
	else if (n <= 93 && n >= 66)
	{
		cm_value = ((n-156)/-9);
	}
	else if (n <= 66 && n >= 40)
	{
		cm_value = ((n-112)/-4.8);
	}
	else if (n <= 40 && n >= 34)
	{
		cm_value = ((n-58)/-1.2);
	}
	else
	{
		cm_value = 25;
	}
	return cm_value;
}

float S2_sensor_value_front(float n) 
{
	float cm_value;
	
	if (n <= 148 && n >= 125)
	{
		cm_value = ((n-240)/-23);
	}
	else if (n <=125 && n >= 109)
	{
		cm_value = ((n-205)/-16);
	}
	else if (n <= 109 && n >= 94)
	{
		cm_value = ((n-199)/-15);
	}
	else if (n <= 94 && n >= 70)
	{
		cm_value = ((n-150)/-8);
	}
	else if (n <= 70 && n >= 49)
	{
		cm_value = ((n-111)/-4.1);
	}
	else if (n <= 49 && n >= 34)
	{
		cm_value = ((n-94)/-3);
	}
	else
	{
		cm_value = 25;
	}
	return cm_value;
}

float S3_sensor_value_front(float n)
{
	float cm_value;
	
	if (n <= 148 && n >= 125)
	{
		cm_value = ((n-240)/-23);
	}
	else if (n <=125 && n >= 109)
	{
		cm_value = ((n-205)/-16);
	}
	else if (n <= 109 && n >= 94)
	{
		cm_value = ((n-199)/-15);
	}
	else if (n <= 94 && n >= 70)
	{
		cm_value = ((n-150)/-8);
	}
	else if (n <= 70 && n >= 49)
	{
		cm_value = ((n-111)/-4.1);
	}
	else if (n <= 49 && n >= 34)
	{
		cm_value = ((n-94)/-3);
	}
	else
	{
		cm_value = 25;
	}
	return cm_value;
}

float S4_sensor_value_front(float n)
{
	float cm_value;
	
	if (n <= 149 && n >= 124)
	{
		cm_value = ((n-249)/-25);
	}
	else if (n <=124 && n >= 107)
	{
		cm_value = ((n-209)/-17);
	}
	else if (n <= 107 && n >= 94)
	{
		cm_value = ((n-185)/-13);
	}
	else if (n <= 94 && n >= 68)
	{
		cm_value = ((n-154.66)/-8.66);
	}
	else if (n <= 68 && n >= 48)
	{
		cm_value = ((n-108)/-4);
	}
	else if (n <= 48 && n >= 36)
	{
		cm_value = ((n-84)/-2.4);
	}
	else
	{
		cm_value = 25;
	}
	return cm_value;
}

float S5_sensor_value_LONG(float n)
{
	float cm_value;
	
	if (n <= 130 && n >= 92)
	{
		cm_value = ((n-206)/-7.6);
	}
	else if (n <= 92 && n >= 72)
	{
		cm_value = ((n-152)/-4);
	}
	else if (n <= 72 && n >= 60)
	{
		cm_value = ((n-117)/-2.4);
	}
	else if (n <= 60 && n >= 53)
	{
		cm_value = ((n-95)/-1.4);
	}
	else if (n <= 53 && n >= 35)
	{
		cm_value = ((n-86)/-1.1);
	}
	else if (n <= 35 && n >= 30)
	{
		cm_value = ((n-50)/-0.25);
	}
	else
	{
		cm_value = 100;
	}
	return cm_value;
}

ISR (ADC_vect)
{
	
	switch (count) 
	{
		
		case(0):
		
		sensor1 = S1_sensor_value_front(ADCH); //Set sensor1 to the converted ADCH value
		count++; //add 1 to count
		ADMUX = (1<<ADLAR)|(1<<REFS0)|(1<<MUX2); //Set ADMUX to PA4
		break;
		case(1):
		
		sensor2 = ADCH; //Set sensor2 to the converted ADCH value
		count++; //add 1 to count
		ADMUX = (1<<ADLAR)|(1<<REFS0)|(1<<MUX2)|(1<<MUX0); //Set ADMUX to PA5
		break;
		case(2):
		
		sensor3 = S3_sensor_value_front(ADCH); //Set sensor3 to the converted ADCH value
		count++; //add 1 to count
		ADMUX = (1<<ADLAR)|(1<<REFS0)|(1<<MUX2)|(1<<MUX1); //Set ADMUX to PA6
		break;
		case(3):
		
		sensor4 = S4_sensor_value_front(ADCH); //Set sensor4 to the converted ADCH value
		count++; //add 1 to count
		ADMUX = (1<<ADLAR)|(1<<REFS0)|(1<<MUX2)|(1<<MUX1)|(1<<MUX0); //Set ADMUX to PA7
		break;
		case(4):
		
		sensor5 = S5_sensor_value_LONG(ADCH); //Set sensor5 to the converted ADCH value
		count = 0; //clear count
		ADMUX = (1<<ADLAR)|(1<<REFS0)|(1<<MUX1)|(1<<MUX0); //Set ADMUX to PA3
		break;
		/*case(5):
		
		angle();
		
		if {-90 > angle < 90}
		
		angular_value = ADCH;
		
		angular_diff = (angular_value - offset) * sensitivity;
		
		angle += angular_diff/1664;
		
		ADMUX = (1<<ADLAR)|(1<<REFS0)|(1<<MUX1); // PA2
		
		} else 
		{   angle=0;
			count=0;
			reflex_count=0;
			send_angle(OK)}
			
		
		break;*/
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
		         }
	
	USART_check //function Alt. av man aktiverar RFID här också
	ser ut som: Läs parallel in från usart och kolla om rätt ID
	om rätt, sätt en pinne kopplad till KOM avbrott
	*/
			}
			}