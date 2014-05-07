#include <stdint.h>
#include <avr/interrupt.h>
#include <math.h>
#include "Sensor_SPI.h"
#include "Sensor_cm_converter.h"
#include "../CommonLibrary/Common.h"
#define Baudrate 2400
#define F_CPU 18432000UL
#define Baud_Prescale ((F_CPU/(Baudrate*16UL))-1)
volatile uint8_t RFID_tag_read[10];
volatile uint8_t RFID_tag_correct[10];
volatile uint8_t RFID_count = 0;
volatile uint8_t RFID_start_read = 0;


volatile unsigned char test;
//volatile uint8_t  sensor1, sensor2, sensor3, sensor4, sensor5;

static volatile int diff_from_middle_corridor; // Fick det inte att fungera med uint8_t
static volatile uint8_t angle_corridor;

static volatile uint8_t angular_rate_value;
static volatile float angular_rate_total = 0;
static volatile float angular_rate_diff;

void init_interrupts()
{
	ACSR = (1>>ACD)|(1>>ACBG)|(1<<ACIE)|(1>>ACIC)|(0>>ACIS0)|(0>>ACIS1);  //ACD=0 AC på, ACBG=0 external comparator pin AIN0, ACIE=1 interrupt enable, ACIC=0 Timer/Counter disabled, ACIS0..1=1 interrupt on rising/falling output edge
	PORTD |= 1<<PORTD6;
	ADMUX = (1<<ADLAR)|(1<<REFS0)|(1<<MUX1)|(1<<MUX0); // Set the ADMUX
	ADCSRA = (1<<ADIE)|(1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0); //Set Prescaler to 128, Set ADCSRA to enable interrupts
	ADCSRA |= (1<<ADSC); //Start ADC
	
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

void init_variable()
{
	reflex_count = 0;
	count = 0;
}

ISR(USART0_RX_vect)
{	
	if (RFID_count==10)
	{
		if (UDR0 == 13)
		{
			for (int i = 0; i <= 9; i++)
			{
				RFID_tag_correct[i] = RFID_tag_read[i];
			}
			//Ifall vi vill kunna identifiera en specifik RFID så ska vi kolla igenom den avlästa RFIDn med ett antal kända RFID-nummer
			/*if (RFID_tag_correct[9] == 68) 
			{
				SPI_sensor_send_data_byte(ID_BYTE_FOUND_RFID, RFID_1);
			}
			*/
			SPI_sensor_send_data_byte(ID_BYTE_FOUND_RFID, 1);
			RFID_count = 0;
			RFID_start_read = 0;
		}
		else
		{
			RFID_count = 0;
		}
	}
 	if (RFID_start_read == 1)
	{
		RFID_tag_read[RFID_count] = UDR0;
		RFID_count++;
	}
	
	if (UDR0 == 10)
	{
		RFID_start_read = 1;
	}
}

ISR (ADC_vect)
{ 
	switch (count)
	{
		
		case(0):
		ir_sensor_data[0] = S1_convert_sensor_value_left_front(ADCH);
		//sensor1 = S1_convert_sensor_value_left_front(ADCH); //sensor1 får det AD-omvandlade värdet
		count++; //adderar 1 till count
		ADMUX = (1<<ADLAR)|(1<<REFS0)|(1<<MUX2); //sätter ADMUX till PA4 för att AD-omvandla left_back IR-sensor
		break;
		
		case(1):
		ir_sensor_data[1] = S2_convert_sensor_value__left_back(ADCH);
		//sensor2 = S2_convert_sensor_value__left_back(ADCH); //sensor2 får det AD-omvandlade värdet
		count++; //adderar 1 till count
		ADMUX = (1<<ADLAR)|(1<<REFS0)|(1<<MUX2)|(1<<MUX0); //Sätter ADMUX till PA5 för att AD-omvandla right_front IR-sensor
		break;
		
		case(2):
		ir_sensor_data[2] = S3_convert_sensor_value_right_front(ADCH);
		//sensor3 = S3_convert_sensor_value_right_front(ADCH); //sensor3 får det AD-omvandlade värdet
		count++; //adderar 1 till count
		ADMUX = (1<<ADLAR)|(1<<REFS0)|(1<<MUX2)|(1<<MUX1); //Sätter ADMUX till PA6 för att AD-omvandla right_back IR-sensor
		break;
		
		case(3):
		ir_sensor_data[3] = S4_convert_sensor_value_right_back(ADCH);
		//sensor4 = S4_convert_sensor_value_right_back(ADCH); //sensor4 får det AD-omvandlade värdet
		count++; //adderar 1 till count
		ADMUX = (1<<ADLAR)|(1<<REFS0)|(1<<MUX2)|(1<<MUX1)|(1<<MUX0); //Sätter ADMUX till PA7 för att AD-omvandla front_long IR-sensor
		break;
		
		case(4):
		ir_sensor_data[4] = S5_convert_sensor_value_front_long(ADCH);
		//sensor5 = S5_convert_sensor_value_front_long(ADCH); //sensor5 får det AD-omvandlade värdet
		count = 0; //nollställer count
		ADMUX = (1<<ADLAR)|(1<<REFS0)|(1<<MUX1)|(1<<MUX0); //Sätter ADMUX till PA3 för att börja om och AD-omvandla left_front IR-sensor
		angle_corridor = 90 - calculate_angle_corridor(ir_sensor_data[0], ir_sensor_data[1], ir_sensor_data[2], ir_sensor_data[3]); //Beräknar vinkeln roboten har i en korridor utifrån centrumlinjen
		diff_from_middle_corridor = calculate_diff_from_middle_corridor(angle_corridor, ir_sensor_data[1], ir_sensor_data[3]); //Beräknar avvikelsen från mitten av från 20cm vilket är mitten av korridoren
		if (diff_from_middle_corridor > 255)
		{
			diff_from_middle_corridor = 255;
		}
		ir_sensor_data[5] = angle_corridor; //lägger in vinkel i korridoren på plats 6 i ir_sensor_data;
		ir_sensor_data[6] = diff_from_middle_corridor;//lägger in avvikelsen på plats 7 i ir_sensor_data;
		break;
		
		case(5):
		if ((-27.5 < angular_rate_total) & (angular_rate_total < 27.5)) //kollar om roboten roterat 90 grader i någon riktning
		{
			angular_rate_value = ADCH; //läser av vinkelhastigheten
			angular_rate_diff = (angular_rate_value - ANGULAR_RATE_OFFSET)*ANGULAR_RATE_SENSITIVITY; //beräknar förändringen av roterad vinkel sedan sedan förra avläsningen
			angular_rate_total += (angular_rate_diff / 10000); //Addera förändringen av roterad vinkel till den totala roterade vinkeln
			//ADMUX = (1<<ADLAR)|(1<<REFS0)|(1<<MUX1); // Sätter ADMUX till PA2 för  avläsning av vinkelhastighetssensorn
		} else
		{
			ADMUX = (1<<ADLAR)|(1<<REFS0)|(1<<MUX1)|(1<<MUX0); //Sätter ADMUX till PA3 så att IR vänster fram börjar AD-omvandlas
			angular_rate_total = 0; //sätter roterad vinkel till 0 när roboten roterat 90 grader i någon riktning
			count = 0; //sätter count till 0 för att återgå till AD-omvandling av IR-sensorerna
			reflex_count = 0; //nollställer avläsningen av avståndet för att kunna påbörja ny avläsning
			SPI_sensor_send_rotation_finished(); //skickar meddelande till KOM att rotationen är klar
		}
		
		break;
	}
	
	ADCSRA |= (1<<ADSC); //startar ny AD-omvandling
	
	
}

ISR(ANALOG_COMP_vect){
	if (PORTD & (1<<PORTD6)){
		PORTD &= 0xbf;
	}
	else{
		PORTD |= (1<<PORTD6);
	}
	reflex_count = reflex_count+1;
	ACSR |= (1<<ACI);
}

//Funktion som omvandlar 
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
		mm_value = (((digital_distance - 101)*100) / -38);
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
		cm_value = (((digital_distance-206)*10) / -76);
	}
	else if (digital_distance <= 92 && digital_distance >= 72)
	{
		cm_value = ((digital_distance-152) / -4);
	}
	else if (digital_distance <= 72 && digital_distance >= 60)
	{
		cm_value = (((digital_distance-117)*10) / -24);
	}
	else if (digital_distance <= 60 && digital_distance >= 53)
	{
		cm_value = (((digital_distance-95)*10) / -14);
	}
	else if (digital_distance <= 53 && digital_distance >= 35)
	{
		cm_value = (((digital_distance-86)*10) / -11);
	}
	else if (digital_distance <= 35 && digital_distance >= 30)
	{
		cm_value = (((digital_distance-50)*10) / -3);
	}
	else
	{
		cm_value = 255;
	}
	return cm_value;
}

uint8_t calculate_angle_corridor(uint8_t sensor1, uint8_t sensor2, uint8_t sensor3, uint8_t sensor4)
{
	uint8_t angle_in_corridor;
	angle_in_corridor = (((((atan2((sensor3-sensor4), 78))*180) / 3.14) + ((((atan2((sensor2-sensor1), 78))*180) / 3.14))) / 2);
	return angle_in_corridor;
}

int calculate_diff_from_middle_corridor(uint8_t angle_corridor, uint8_t sensor2, uint8_t sensor4)
{
	int diff_from_middle_corridor_mm;
	diff_from_middle_corridor_mm = (cos(((angle_corridor*3.14) / 180.0f) - 1.57))*((100 - tan((angle_corridor*3.14 / 180.0f) - 1.57)*38 + sensor4)) - (((cos(((angle_corridor*3.14) / 180.0f) - 1.57))*((100 - tan((angle_corridor*3.14 / 180.0f) - 1.57)*38 + sensor4)) + (cos(((angle_corridor*3.14) / 180.0f) - 1.57))*((100 + tan((angle_corridor*3.14 / 180.0f) - 1.57)*38 + sensor2))) / 2) + 200; //avvikelsen från mitten i en korridor
	return diff_from_middle_corridor_mm;
}