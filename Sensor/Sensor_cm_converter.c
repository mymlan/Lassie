#include <stdint.h>
#include <avr/interrupt.h>
#include <math.h>
#include "Sensor_SPI.h"
#include "Sensor_cm_converter.h"
#include "../CommonLibrary/Common.h"
#define Baudrate 2400
#define F_CPU 18432000UL
#define Baud_Prescale ((F_CPU/(Baudrate*16UL))-1)
volatile uint8_t RFIDtag_read[10];
volatile uint8_t RFIDtag_correct[10];
volatile uint8_t RFID_count = 0;
volatile uint8_t Startbit_hittad = 0;

volatile uint16_t reflex_count = 0;
volatile unsigned char test;
volatile uint8_t sensor1, sensor2, sensor3, sensor4, sensor5;

volatile uint8_t count=0;

volatile int diff_from_middle_corridor; // fick det inte att fungera med uint8_t
volatile uint8_t angle_corridor;

volatile uint8_t angular_rate_value;
volatile float angular_rate_total = 0;
volatile float angular_rate_diff;

void init_interrupts()
{
	ACSR = (1>>ACD)|(1>>ACBG)|(1<<ACIE)|(1>>ACIC)|(0>>ACIS0)|(0>>ACIS1);  //ACD=0 AC på, ACBG=0 external comparator pin AIN0, ACIE=1 interrupt enable, ACIC=0 Timer/Counter disabled, ACIS0..1=1 interrupt on rising/falling output edge
	//DDRD = 1<<DDD6;
	PORTD |= 1<<PORTD6;
	ADMUX = (1<<ADLAR)|(1<<REFS0)|(1<<MUX1)|(1<<MUX0); // Set the ADMUX
	ADCSRA = (1<<ADIE)|(1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0); //Set Prescaler to 128, Set ADCSRA to enable interrupts
	
	ADCSRA |= (1<<ADSC); //Start ADC
	//DIDR1 = (1<<AIN1D)|(1<<AIN0D); //
}

void USART_init(){
	DDRD |= (1<<DDD2);  //Enable RFID
	UCSR0B |= (1<<RXEN0);  //Enable USART0 receiver
	UCSR0B |= (1<<RXCIE0);  //enable receive complete interrupt
	UCSR0C |= (1<<UCSZ00) | (1<<UCSZ01) | (1<<UCSZ02);  //8 bits per frame
	UCSR0C |= (0<<USBS0);  //Declares one stop bit
	//eventuellt prescale räkning på buad?!?!?!
	UBRR0H = (unsigned char)(Baud_Prescale>>8);//set upper 8 bit of baudrate
	UBRR0L = (unsigned char)Baud_Prescale; //set rest of bits of baudrate
	sei();
}

ISR(USART0_RX_vect)
{	
	if (RFID_count==10)
	{
		if (UDR0 == 13)
		{
			for (int i = 0; i <= 9; i++)
			{
				RFIDtag_correct[i] = RFIDtag_read[i];
			}
			RFID_count = 0;
			Startbit_hittad = 0;
			//flagga till kom
		}
		else
		{
			RFID_count = 0;
		}
	}
 	if (Startbit_hittad == 1)
	{
		RFIDtag_read[RFID_count] = UDR0;
		RFID_count++;
	}
	
	if (UDR0 == 10)
	{
	Startbit_hittad = 1;
	}
	//while ((UCSR0A &(1<<RXC0))==0){ //check if receiving is complete
	//}
}

ISR (ADC_vect)
{
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
		ir_sensor_data[2] = S3_convert_sensor_value_right_front(ADCH);
		sensor3 = S3_convert_sensor_value_right_front(ADCH); //sensor3 får det AD-omvandlade värdet
		count++; //adderar 1 till count
		ADMUX = (1<<ADLAR)|(1<<REFS0)|(1<<MUX2)|(1<<MUX1); //Sätter ADMUX till PA6
		break;
		
		case(3):
		ir_sensor_data[3] = S4_convert_sensor_value_right_back(ADCH);
		sensor4 = S4_convert_sensor_value_right_back(ADCH); //sensor4 får det AD-omvandlade värdet
		count++; //adderar 1 till count
		ADMUX = (1<<ADLAR)|(1<<REFS0)|(1<<MUX2)|(1<<MUX1)|(1<<MUX0); //Sätter ADMUX till PA7
		break;
		
		case(4):
		ir_sensor_data[4] = S5_convert_sensor_value_front_long(ADCH);
		sensor5 = S5_convert_sensor_value_front_long(ADCH); //sensor5 får det AD-omvandlade värdet
		count = 0; //nollställer count
		ADMUX = (1<<ADLAR)|(1<<REFS0)|(1<<MUX1)|(1<<MUX0); //Sätter ADMUX till PA3
		angle_corridor = 90 - (((((atan2((sensor3-sensor4), 78))*180) / 3.14) + ((((atan2((sensor2-sensor1), 78))*180) / 3.14))) / 2);
		diff_from_middle_corridor = ((cos(((angle_corridor*3.14) / 180.0f) - 1.57))*((100 - tan((angle_corridor*3.14 / 180.0f) - 1.57)*38 + sensor4))) - (((cos(((angle_corridor*3.14) / 180.0f) - 1.57))*((100 - tan((angle_corridor*3.14 / 180.0f) - 1.57)*38 + sensor4)) + (cos(((angle_corridor*3.14) / 180.0f) - 1.57))*((100 + tan((angle_corridor*3.14 / 180.0f) - 1.57)*38 + sensor2))) / 2) + 200; //Beräknar avståndet från den väggen i en korridor 
		if (diff_from_middle_corridor > 255)
		{
			diff_from_middle_corridor = 255;
		}
		ir_sensor_data[5] = angle_corridor;//angle_corridor;
		ir_sensor_data[6] = diff_from_middle_corridor;//diff_from_middle_corridor;
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

 uint8_t S1_convert_sensor_value_left_front(uint8_t digital_distance)
{
	uint8_t mm_value;
	
	if (digital_distance <= 144 && digital_distance >= 104)
	{
		mm_value = ((digital_distance - 224) / -2);
	}
	else if (digital_distance <=104 && digital_distance >= 80)
	{
		mm_value = (((digital_distance - 176)*10) / -12);
	}
	else if (digital_distance <= 80 && digital_distance >= 66)
	{
		mm_value = ((digital_distance - 136)*10/ -7);
	}
	else if (digital_distance <= 66 && digital_distance >= 55)
	{
		mm_value = (((digital_distance - 121)*100) / -55);
	}
	else if (digital_distance <= 55 && digital_distance >= 47)
	{
		mm_value = (((digital_distance - 103)*10) / -4);
	}
	else if (digital_distance <= 47 && digital_distance >= 43)
	{
		mm_value = (((digital_distance - 75)*10) / -2);
	}
	else if (digital_distance <= 43 && digital_distance >= 37)
	{
		mm_value = (((digital_distance - 91)*10) / -3);
	}
	else if (digital_distance <= 37 && digital_distance >= 30)
	{
		mm_value = (((digital_distance - 68)*100) / -17);
	}
	else
	{
		mm_value = 255;
	}
	return mm_value;
}

uint8_t S2_convert_sensor_value__left_back(uint8_t digital_distance)
{
	uint8_t mm_value;
	
	if (digital_distance <= 142 && digital_distance >= 103)
	{
		mm_value = (((digital_distance - 220)*100) / -195);
	}
	else if (digital_distance <=103 && digital_distance >= 80)
	{
		mm_value = (((digital_distance - 172)*100) / -172);
	}
	else if (digital_distance <= 80 && digital_distance >= 65)
	{
		mm_value = (((digital_distance - 140)*100) / -75);
	}
	else if (digital_distance <= 65 && digital_distance >= 55)
	{
		mm_value = (((digital_distance - 115)*10) / -5);
	}
	else if (digital_distance <= 55 && digital_distance >= 47)
	{
		mm_value = (((digital_distance - 103)*10) / -4);
	}
	else if (digital_distance <= 47 && digital_distance >= 41)
	{
		mm_value = (((digital_distance - 89)*10) / -3);
	}
	else if (digital_distance <= 41 && digital_distance >= 35)
	{
		mm_value = (((digital_distance - 53)*10) / -3);
	}
	else if (digital_distance <= 35 && digital_distance >= 31)
	{
		mm_value = (((digital_distance - 42)*10) / -2);
	}
	else
	{
		mm_value = 255;
	}
	return mm_value;
}

uint8_t S3_convert_sensor_value_right_front(uint8_t digital_distance)
{
	uint8_t mm_value;
	
	if (digital_distance <= 150 && digital_distance >= 108)
	{
		mm_value = (((digital_distance - 234)*10) / -21);
	}
	else if (digital_distance <=108 && digital_distance >= 83)
	{
		mm_value = (((digital_distance - 183)*100) / -125);
	}
	else if (digital_distance <= 83 && digital_distance >= 68)
	{
		mm_value = (((digital_distance - 143)*100) / -75);
	}
	else if (digital_distance <= 68 && digital_distance >= 56)
	{
		mm_value = (((digital_distance - 128)*10) / -6);
	}
	else if (digital_distance <= 56 && digital_distance >= 41)
	{
		mm_value = (((digital_distance - 101)*1000) / -375);
	}
	else if (digital_distance <=41 && digital_distance >= 30)
	{
		mm_value = (((digital_distance - 70)*100) / -18);
	}
	else
	{
		mm_value = 255;
	}
	return mm_value;
}

uint8_t S4_convert_sensor_value_right_back(uint8_t digital_distance)
{
	int mm_value;
	
	if (digital_distance <= 155 && digital_distance >= 110)
	{
		mm_value = (((digital_distance - 245)*100) / -225);
	}
	else if (digital_distance <=110 && digital_distance >= 84)
	{
		mm_value = (((digital_distance - 188)*10) / -13);
	}
	else if (digital_distance <= 84 && digital_distance >= 68)
	{
		mm_value = (((digital_distance - 148)*10) / -8);
	}
	else if (digital_distance <= 68 && digital_distance >= 56)
	{
		mm_value = (((digital_distance - 128)*10) / -6);
	}
	else if (digital_distance <= 56 && digital_distance >= 48)
	{
		mm_value = (((digital_distance - 104)*10) / -4);
	}
	else if (digital_distance <= 48 && digital_distance >= 40)
	{
		mm_value = (((digital_distance - 104)*10) / -4);
	}
	else if (digital_distance <= 40 && digital_distance >= 29)
	{
		mm_value = (((digital_distance - 64)*100) / -15);
	}
	else
	{
		mm_value = 255;
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