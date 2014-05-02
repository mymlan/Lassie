#include <stdint.h>
#include <avr/interrupt.h>
#include <math.h>
#include "Sensor_SPI.h"
#include "Sensor_cm_converter.h"
#include "../CommonLibrary/Common.h"

volatile uint16_t reflex_count = 0;
volatile unsigned char test;
int baud;
volatile uint8_t sensor1, sensor2, sensor3, sensor4, sensor5;

uint8_t ir_sensor_data[7]; // volatile fungerar inte med send funktionen

volatile uint8_t count=0;

volatile int diff_from_middle_corridor; // fick det inte att fungera med uint8_t
volatile uint8_t angle_corridor;
volatile float a;
volatile float b;

volatile uint8_t angular_rate_value;
volatile float angular_rate_total = 0;
volatile float angular_rate_diff;

void init_interrupts()
{
	ACSR = (1>>ACD)|(1>>ACBG)|(1<<ACIE)|(1>>ACIC)|(0>>ACIS0)|(0>>ACIS1);  //ACD=0 AC på, ACBG=0 external comparator pin AIN0, ACIE=1 interrupt enable, ACIC=0 Timer/Counter disabled, ACIS0..1=1 interrupt on rising output edge
	//DDRD = 1<<DDD6;
	PORTD = 1<<PORTD6;
	ADMUX = (1<<ADLAR)|(1<<REFS0)|(1<<MUX1)|(1<<MUX0); // Set the ADMUX
	ADCSRA = (1<<ADIE)|(1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0); //Set Prescaler to 128, Set ADCSRA to enable interrupts
	
	ADCSRA |= (1<<ADSC); //Start ADC
	//DIDR1 = (1<<AIN1D)|(1<<AIN0D); //
}

ISR (ADC_vect)
{
	//count = 5;
	
	switch (count)
	{
		
		case(0):
		ir_sensor_data[0] = S1_convert_sensor_value_left_front(ADCH);
		sensor1 = 50;//S1_convert_sensor_value_left_front(ADCH); //sensor1 får det AD-omvandlade värdet
		count++; //adderar 1 till count
		ADMUX = (1<<ADLAR)|(1<<REFS0)|(1<<MUX2); //sätter ADMUX till PA4
		break;
		
		case(1):
		ir_sensor_data[1] = S2_convert_sensor_value__left_back(ADCH);
		sensor2 = 50;//S2_convert_sensor_value__left_back(ADCH); //sensor2 får det AD-omvandlade värdet
		count++; //adderar 1 till count
		ADMUX = (1<<ADLAR)|(1<<REFS0)|(1<<MUX2)|(1<<MUX0); //Sätter ADMUX till PA5
		break;
		
		case(2):
		ir_sensor_data[2] = S3_convert_sensor_value_right_front(ADCH);
		sensor3 = 190;//S3_convert_sensor_value_right_front(ADCH); //sensor3 får det AD-omvandlade värdet
		count++; //adderar 1 till count
		ADMUX = (1<<ADLAR)|(1<<REFS0)|(1<<MUX2)|(1<<MUX1); //Sätter ADMUX till PA6
		break;
		
		case(3):
		ir_sensor_data[3] = S4_convert_sensor_value_right_back(ADCH);
		sensor4 = 245;//S4_convert_sensor_value_right_back(ADCH); //sensor4 får det AD-omvandlade värdet
		count++; //adderar 1 till count
		ADMUX = (1<<ADLAR)|(1<<REFS0)|(1<<MUX2)|(1<<MUX1)|(1<<MUX0); //Sätter ADMUX till PA7
		break;
		
		case(4):
		ir_sensor_data[4] = S5_convert_sensor_value_front_long(ADCH);
		sensor5 = S5_convert_sensor_value_front_long(ADCH); //sensor5 får det AD-omvandlade värdet
		count = 0; //nollställer count
		ADMUX = (1<<ADLAR)|(1<<REFS0)|(1<<MUX1)|(1<<MUX0); //Sätter ADMUX till PA3
		angle_corridor = 90 - ((((atan2((sensor3-sensor4), 105))*180) / 3.14)  + (((atan2((sensor2-sensor1), 105)*180)) / 3.14) / 2); //Ger vinkel från vänstra väggen
		diff_from_middle_corridor = (cos(((angle_corridor*3.14) / 180.0f) - 1.57))*((100 - tan((angle_corridor*3.14 / 180.0f) - 1.57)*55 + sensor4)); //Beräknar avståndet från den väggen i en korridor
		ir_sensor_data[5] = angle_corridor;
		ir_sensor_data[6] = diff_from_middle_corridor;
		break;
		
		case(5):
		if ((-20 < angular_rate_total) & (angular_rate_total < 20))
		{
			angular_rate_value = ADCH;
			angular_rate_diff = (angular_rate_value - ANGULAR_RATE_OFFSET)*ANGULAR_RATE_SENSITIVITY;
			angular_rate_total += (angular_rate_diff / 10000);
			ADMUX = (1<<ADLAR)|(1<<REFS0)|(1<<MUX1); // Sätter ADMUX till PA2
		} else
		{
			angular_rate_total = 0;
			count = 0;
			reflex_count = 0;
			SPI_sensor_send_rotation_finished();
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

//**************** GUSTAV TERRITORIUM **************

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

//************ SLUT PÅ TERRITORIUM *******************


 uint8_t S1_convert_sensor_value_left_front(uint8_t digital_distance)
{
	uint8_t mm_value;
	
	if (digital_distance <= 135 && digital_distance >= 114)
	{
		mm_value = (((digital_distance - 135)*10) / -21);
	}
	else if (digital_distance <=114 && digital_distance >= 97)
	{
		mm_value = (((digital_distance - 131)*10) / -17);
	}
	else if (digital_distance <= 97 && digital_distance >= 77)
	{
		mm_value = ((digital_distance - 117) / -1);
	}
	else if (digital_distance <= 77 && digital_distance >= 62)
	{
		mm_value = (((digital_distance - 107)*100) / -75);
	}
	else if (digital_distance <= 62 && digital_distance >= 50)
	{
		mm_value = (((digital_distance - 86)*10) / -4);
	}
	else if (digital_distance <= 50 && digital_distance >= 35)
	{
		mm_value = (((digital_distance - 77)*10) / -3);
	}
	else if (digital_distance <= 35 && digital_distance >= 26)
	{
		mm_value = (((digital_distance - 56)*100) / -15);
	}
	else
	{
		mm_value = 250;
	}
	return mm_value;
}

uint8_t S2_convert_sensor_value__left_back(uint8_t digital_distance)
{
	uint8_t mm_value;
	
	if (digital_distance <= 130 && digital_distance >= 112)
	{
		mm_value = (((digital_distance - 130)*10) / -18);
	}
	else if (digital_distance <=112 && digital_distance >= 96)
	{
		mm_value = (((digital_distance - 128)*10) / -16);
	}
	else if (digital_distance <= 96 && digital_distance >= 73)
	{
		mm_value = (((digital_distance - 119)*100) / -115);
	}
	else if (digital_distance <= 73 && digital_distance >= 58)
	{
		mm_value = (((digital_distance - 103)*100) / -75);
	}
	else if (digital_distance <= 58 && digital_distance >= 41)
	{
		mm_value = (((digital_distance - 92)*100) / -57);
	}
	else if (digital_distance <= 41 && digital_distance >= 26)
	{
		mm_value = (((digital_distance - 68)*10) / -3);
	}
	else if (digital_distance <= 26 && digital_distance >= 18)
	{
		mm_value = (((digital_distance - 45)*100) / -13);
	}
	else
	{
		mm_value = 250;
	}
	return mm_value;
}

uint8_t S3_convert_sensor_value_right_front(uint8_t digital_distance)
{
	uint8_t mm_value;
	
	if (digital_distance <= 130 && digital_distance >= 95)
	{
		mm_value = (((digital_distance - 130)*100) / -175);
	}
	else if (digital_distance <=95 && digital_distance >= 75)
	{
		mm_value = ((digital_distance - 115) / -1);
	}
	else if (digital_distance <= 75 && digital_distance >= 62)
	{
		mm_value = (((digital_distance - 101)*100) / -65);
	}
	else if (digital_distance <= 62 && digital_distance >= 46)
	{
		mm_value = (((digital_distance - 94)*100) / -53);
	}
	else if (digital_distance <= 46 && digital_distance >= 33)
	{
		mm_value = (((digital_distance - 67)*100) / -26);
	}
	else if (digital_distance <= 33 && digital_distance >= 23)
	{
		mm_value = (((digital_distance - 56)*100) / -17);
	}
	else
	{
		mm_value = 250;
	}
	return mm_value;
}

uint8_t S4_convert_sensor_value_right_back(uint8_t digital_distance)
{
	int mm_value;
	
	if (digital_distance <= 135 && digital_distance >= 104)
	{
		mm_value = (((digital_distance - 135)*100) / -155);
	}
	else if (digital_distance <=104 && digital_distance >= 79)
	{
		mm_value = (((digital_distance - 129)*100) / -125);
	}
	else if (digital_distance <= 79 && digital_distance >= 63)
	{
		mm_value = (((digital_distance - 110)*10) / -8);
	}
	else if (digital_distance <= 63 && digital_distance >= 50)
	{
		mm_value = (((digital_distance - 88)*100) / -43);
	}
	else if (digital_distance <= 50 && digital_distance >= 36)
	{
		mm_value = (((digital_distance - 75)*100) / -28);
	}
	else if (digital_distance <= 36 && digital_distance >= 24)
	{
		mm_value = (((digital_distance - 64)*10) / -2);
	}
	else
	{
		mm_value = 250;
	}
	return mm_value;
}

uint8_t S5_convert_sensor_value_front_long(uint8_t digital_distance)
{
	int cm_value;
	
	if (digital_distance <= 130 && digital_distance >= 92)
	{
		cm_value = ((digital_distance-206)/-7.6);
	}
	else if (digital_distance <= 92 && digital_distance >= 72)
	{
		cm_value = ((digital_distance-152)/-4);
	}
	else if (digital_distance <= 72 && digital_distance >= 60)
	{
		cm_value = ((digital_distance-117)/-2.4);
	}
	else if (digital_distance <= 60 && digital_distance >= 53)
	{
		cm_value = ((digital_distance-95)/-1.4);
	}
	else if (digital_distance <= 53 && digital_distance >= 35)
	{
		cm_value = ((digital_distance-86)/-1.1);
	}
	else if (digital_distance <= 35 && digital_distance >= 30)
	{
		cm_value = ((digital_distance-50)/-0.25);
	}
	else
	{
		cm_value = 100;
	}
	return cm_value;
}