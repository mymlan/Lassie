
#define F_CPU (18432000L)
#define Baudrate 2400  //Bits per second hos RFID
#define Baud_Prescale ((F_CPU/(Baudrate*16UL))-1)  //Baud rate generator (prescaler) för USART

#include <util/delay.h>
#include <stdint.h>
#include <avr/interrupt.h>
#include <math.h>
#include "Sensor_SPI.h"
#include "Sensor_cm_converter.h"
#include "../CommonLibrary/Common.h"

//-------------VARIABLER/KONSTANTER---------------//
// tillhörande RFID
volatile uint8_t RFID_tag_read[10];  //Array med 10 bytes där RFID ID lagras
volatile uint8_t RFID_tag_correct[10];  //Avläst ID på RFID läggs in här om korrekt avläsning skett
volatile uint8_t RFID_count = 0;  //Räknare för att kontrollera att rätt antal bytes lästs in från RFID
volatile uint8_t RFID_start_read = 0;  //Kontrollerar startbit innan bytes lägg in på RFID_tag_read

// tillhörande vinkelhastighetssensorn
static volatile double angular_rate_offset;
static volatile uint8_t angular_rate_value;
static volatile float angular_rate_total;
static volatile float angular_rate_diff;
static float ANGULAR_RATE_SENSITIVITY = 1.28;

// tillhörande kalibrering av vinkelhastighetssensorn
static float angular_read[100];

// ingår i sensor_data
static volatile uint8_t sensor_data_distance_from_right_wall;
static volatile uint8_t sensor_data_angle_corridor;

// tillhör tidigare reglering, bortkommenterad.
// static uint8_t SIDE_IR_DISTANCE = 78;
// static uint8_t HALF_ROBOT_LENGTH = 100;

//-----------------STATIC FUNKTIONER----------------//
static float Sum_array(float a[], int num_elements)
{
	int i, sum=0;
	for (i=0; i<num_elements; i++)
	{
		sum = sum + a[i];
	}
	return(sum);
}

//----INITIERINGSFUNKTION/KALIBRERINGSFUNKTION-----//
void Sensor_init_AD_converter()
{
	ADMUX = (1<<ADLAR)|(1<<REFS0)|(1<<MUX1)|(1<<MUX0); // Set the ADMUX
	ADCSRA = (1<<ADIE)|(1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0); //Set Prescaler to 128, Set ADCSRA to enable interrupts
	ADCSRA |= (1<<ADSC); //Start ADC
	
	next_sensor_to_be_converted = IR_LEFT_FRONT;
}

void Sensor_init_analog_comparator()
{
	COMMON_CLEAR_BIT(ACSR, ACD); //ACD=0 AC på
	COMMON_CLEAR_BIT(ACSR, ACBG); //ACBG=0 external comparator pin AIN0
	COMMON_CLEAR_BIT(ACSR, ACIC); //ACIC=0 Timer/Counter disabled
	COMMON_CLEAR_BIT(ACSR, ACIS0); //ACIS0..1=0 interrupt on rising/falling output edge
	COMMON_CLEAR_BIT(ACSR, ACIS1);
	COMMON_SET_BIT(ACSR, ACIE); //ACIE=1 interrupt enable
	COMMON_SET_BIT(DDRD, DDD6);  //PORTD6 som output
	COMMON_SET_BIT(PORTD, PORTD6);  //Sätter PORTD6 (pinne 20) till HIGH som används till spänningsdelaren hos reflexsensorn
	
	reflex_count = 0;
	number_of_reflex_counts_to_RFID = 0;
	number_of_reflex_counts_to_RFID_requested = 0;
}

void Sensor_init_USART_for_RFID()
{
	COMMON_CLEAR_PIN(PORTD, PORTD2);
	COMMON_SET_BIT(DDRD, DDD2);  //Enable RFID
	//DDRD |= (1<<DDD2);  
	COMMON_SET_BIT(UCSR0B, RXEN0);  //Enable USART0 receiver
	COMMON_SET_BIT(UCSR0B, RXCIE0);  //enable receive complete interrupt
	//8 bits frame format---
	COMMON_CLEAR_BIT(UCSR0B, UCSZ02);
	COMMON_SET_BIT(UCSR0C, UCSZ00);
	COMMON_SET_BIT(UCSR0C, UCSZ01);
	//-----------------------  
	//UCSR0C |= (1<<UCSZ00) | (1<<UCSZ01) | (1<<UCSZ02);  //8 bits per frame
	COMMON_SET_BIT(UCSR0C, USBS0);  //Declares one stop bit
	UBRR0H = (unsigned char)(Baud_Prescale>>8);//set upper 8 bit of baudrate
	UBRR0L = (unsigned char)Baud_Prescale; //set rest of bits of baudrate
}

void Sensor_init_button_for_calibrating_angular_rate_sensor()
{
	COMMON_CLEAR_PIN(DDRC, DDC0);
	COMMON_SET_PIN(PORTC, PORTC0);
	COMMON_SET_BIT(PCICR, PCIE2);
	COMMON_SET_BIT(PCMSK2, PCINT16);
	
	angular_rate_offset = 133;
	angular_rate_value = 0;
	angular_rate_total = 0;
	angular_rate_diff = 0;
}

void Calibrate_angular_rate_sensor()
{
	ADMUX = (1<<ADLAR)|(1<<REFS0)|(1<<MUX1);
	_delay_ms(20);
	ADCSRA |= (1<<ADSC);
	
	for (int i = 0; i < 100; i++)
	{
		_delay_ms(5);
		angular_read[i] = ADCH;
		ADCSRA |= (1<<ADSC);
	}
	angular_rate_offset = Sum_array(angular_read,100) / 100.0f;
}

//----------------AVBROTTSVEKTORER----------------//
ISR(PCINT2_vect)
{
	Calibrate_angular_rate_sensor();
}

ISR(USART0_RX_vect)
{	
	//SPI_sensor_send_data_byte(ID_BYTE_FOUND_RFID, 1);

	if (RFID_count==10)  //kollar om korrekt antal bitar
	{
		if (UDR0 == 13)  //kollar om korrekt stopbit
		{
			for (int i = 0; i <= 9; i++)
			{
				RFID_tag_correct[i] = RFID_tag_read[i];  //Laddar över korrekt avläst RFID ID
			}
			//Ifall vi vill kunna identifiera en specifik RFID så ska vi kolla igenom den avlästa RFIDn med ett antal kända RFID-nummer
			/*if (RFID_tag_correct[9] == 68) 
			{
				SPI_sensor_send_data_byte(ID_BYTE_FOUND_RFID, RFID_1);
			}*/
			

			
			SPI_sensor_send_data_byte(ID_BYTE_FOUND_RFID, 1);  //Meddelar att RFID hittats (samt vilken RFID som hittats)

			RFID_count = 0; 
			RFID_start_read = 0;
		}
		else
		{
			RFID_count = 0;
		}
	}
	//Lägger in avlästa bytes om korrekt startbit hittats
 	if (RFID_start_read == 1)  
	{
		RFID_tag_read[RFID_count] = UDR0;
		RFID_count++;
	}
	//Meddelar resten av ISR att en korrekt startbit hittats
	if (UDR0 == 10)
	{
		//SPI_sensor_send_data_byte(ID_BYTE_FOUND_RFID, 1);  //Meddelar att RFID hittats (samt vilken RFID som hittats)
		//RFID_count = 0;
		//RFID_start_read = 0;
		RFID_start_read = 1;
	}
}

ISR (ADC_vect)
{ 
	switch (next_sensor_to_be_converted)
	{
		case(IR_LEFT_FRONT):
			ir_sensor_data[0] = S1_convert_sensor_value_left_front(ADCH) - 25;
			next_sensor_to_be_converted = IR_LEFT_BACK; //adderar 1 till sensor_which_AD_converted
			ADMUX = (1<<ADLAR)|(1<<REFS0)|(1<<MUX2); //sätter ADMUX till PA4 för att AD-omvandla left_back IR-sensor
			break;
		case(IR_LEFT_BACK):
			ir_sensor_data[1] = S2_convert_sensor_value_left_back(ADCH) - 25;
			next_sensor_to_be_converted = IR_RIGHT_FRONT; //adderar 1 till sensor_which_AD_converted
			ADMUX = (1<<ADLAR)|(1<<REFS0)|(1<<MUX2)|(1<<MUX0); //Sätter ADMUX till PA5 för att AD-omvandla right_front IR-sensor
			break;
		case(IR_RIGHT_FRONT):
			ir_sensor_data[2] = S3_convert_sensor_value_right_front(ADCH) - 25;
			next_sensor_to_be_converted = IR_RIGHT_BACK; //adderar 1 till sensor_which_AD_converted
			ADMUX = (1<<ADLAR)|(1<<REFS0)|(1<<MUX2)|(1<<MUX1); //Sätter ADMUX till PA6 för att AD-omvandla right_back IR-sensor
			break;
		case(IR_RIGHT_BACK):
			ir_sensor_data[3] = S4_convert_sensor_value_right_back(ADCH) - 25;
			next_sensor_to_be_converted = IR_FRONT_LONG; //adderar 1 till sensor_which_AD_converted
			ADMUX = (1<<ADLAR)|(1<<REFS0)|(1<<MUX2)|(1<<MUX1)|(1<<MUX0); //Sätter ADMUX till PA7 för att AD-omvandla front_long IR-sensor
			break;
		case(IR_FRONT_LONG):
			ir_sensor_data[4] = S5_convert_sensor_value_front_long(ADCH); //S5_convert_sensor_value_front_long(ADCH);
			next_sensor_to_be_converted = IR_LEFT_FRONT; //nollställer sensor_which_AD_converted
			ADMUX = (1<<ADLAR)|(1<<REFS0)|(1<<MUX1)|(1<<MUX0); //Sätter ADMUX till PA3 för att börja om och AD-omvandla left_front IR-sensor
			sensor_data_angle_corridor = Calculate_approx_angle_corridor(ir_sensor_data[0], ir_sensor_data[1], ir_sensor_data[2], ir_sensor_data[3]); //Beräknar vinkeln roboten har i en korridor utifrån centrumlinjen
			sensor_data_distance_from_right_wall = Calculate_distance_from_right_wall(ir_sensor_data[0], ir_sensor_data[1], ir_sensor_data[2], ir_sensor_data[3]); //Beräknar avvikelsen från mitten av från 20cm vilket är mitten av korridoren
			//sensor_data_distance_from_right_wall = calculate_diff_from_middle_corridor(sensor_data_angle_corridor, ir_sensor_data[0], ir_sensor_data[1], ir_sensor_data[2], ir_sensor_data[3]); //Beräknar avvikelsen från mitten av från 20cm vilket är mitten av korridoren
			//if (sensor_data_distance_from_right_wall > 255)
			//{
			//	sensor_data_distance_from_right_wall = 255;
			//}
			ir_sensor_data[5] = sensor_data_angle_corridor; //lägger in vinkel i korridoren på plats 6 i ir_sensor_data;
			ir_sensor_data[6] = sensor_data_distance_from_right_wall;//lägger in avvikelsen på plats 7 i ir_sensor_data;
			break;
		case(ANGULAR_RATE):
			if ((-23 < angular_rate_total) && (angular_rate_total < 23)) //kollar om roboten roterat 90 grader i någon riktning, positiv i vänster riktning
			{
				angular_rate_value = ADCH; //läser av vinkelhastigheten
				angular_rate_diff = (angular_rate_value - angular_rate_offset)*ANGULAR_RATE_SENSITIVITY; //beräknar förändringen av roterad vinkel sedan sedan förra avläsningen
				angular_rate_total += (angular_rate_diff / 10000); //Addera förändringen av roterad vinkel till den totala roterade vinkeln
				
				ADMUX = (1<<ADLAR)|(1<<REFS0)|(1<<MUX1); // Sätter ADMUX till PA2 för  avläsning av vinkelhastighetssensorn
			}
			else
			{
				ADMUX = (1<<ADLAR)|(1<<REFS0)|(1<<MUX1)|(1<<MUX0); //Sätter ADMUX till PA3 så att IR vänster fram börjar AD-omvandlas
				angular_rate_total = 0; //sätter roterad vinkel till 0 när roboten roterat 90 grader i någon riktning
				next_sensor_to_be_converted = IR_LEFT_FRONT; //sätter count till 0 för att återgå till AD-omvandling av IR-sensorerna
				reflex_count = 0; //nollställer avläsningen av avståndet för att kunna påbörja ny avläsning
				SPI_sensor_send_rotation_finished(); //skickar meddelande till KOM att rotationen är klar
				//ACSR |= (1>>ACD); //Startar Analog Comparator (reflexsensorn) // BUGG !!!!!
			}
			break;
		default:
			break;
	}
	
	ADCSRA |= (1<<ADSC); //startar ny AD-omvandling
	
	
}

ISR(ANALOG_COMP_vect)
{
	//Tröskelvärdet höjs resp. sänks om interruptet startas på låg resp. hög 
	if ((PORTD & (1<<PORTD6)) == 64)
	{ 
		COMMON_CLEAR_PIN(PORTD,PORTD6);
	}
	else
	{
		COMMON_SET_PIN(PORTD, PORTD6);  
	}
	reflex_count++;  //Räknar upp 
	COMMON_SET_BIT(ACSR, ACI);  //Tar bort eventuella interrupts på kö (endast ett ska räknas varje gång)
	
	if((number_of_reflex_counts_to_RFID_requested == 1) && (reflex_count == number_of_reflex_counts_to_RFID))
	{
		//skicka tillbaka att sökt avstånd är uppnådd.
		number_of_reflex_counts_to_RFID_requested = 0; 
		reflex_count = 0;
	}
}

//------------------FUNKTIONER--------------------//

//Funktion som beräknar vinkeln roboten har i en korridor
/*
uint8_t Calculate_angle_corridor(uint8_t left_front, uint8_t left_back, uint8_t right_front, uint8_t right_back)
{
	int8_t angle_in_corridor_right;
	int8_t angle_in_corridor_left;
	
	angle_in_corridor_right = atan2(right_back - right_front, SIDE_IR_DISTANCE) * 180 / 3.14;
	angle_in_corridor_left = atan2(left_front - left_back, SIDE_IR_DISTANCE) * 180 / 3.14;
	
	if((left_back > SIDE_SENSOR_OPEN_LIMIT || left_front > SIDE_SENSOR_OPEN_LIMIT) && (right_front > SIDE_SENSOR_OPEN_LIMIT || right_back > SIDE_SENSOR_OPEN_LIMIT))
	{
		return 90;
	}
	else if(left_front > SIDE_SENSOR_OPEN_LIMIT || left_back > SIDE_SENSOR_OPEN_LIMIT)
	{
		return 90 + angle_in_corridor_right;
	}
	else if(right_front > SIDE_SENSOR_OPEN_LIMIT || right_back > SIDE_SENSOR_OPEN_LIMIT)
	{
		return 90 + angle_in_corridor_left;
	}
	else
	{
		return 90 + (angle_in_corridor_right + angle_in_corridor_left) / 2;
	}
}*/

uint8_t Calculate_approx_angle_corridor(uint8_t left_front, uint8_t left_back, uint8_t right_front, uint8_t right_back)
{
	int8_t angle_in_corridor_right;
	int8_t angle_in_corridor_left;
	
	angle_in_corridor_right = right_front - right_back;
	angle_in_corridor_left = left_back - left_front;
	
	if((left_back > SIDE_SENSOR_OPEN_LIMIT || left_front > SIDE_SENSOR_OPEN_LIMIT) && (right_front > SIDE_SENSOR_OPEN_LIMIT || right_back > SIDE_SENSOR_OPEN_LIMIT))
	{
		return 100;
	}
	else if(left_front > SIDE_SENSOR_OPEN_LIMIT || left_back > SIDE_SENSOR_OPEN_LIMIT)
	{
		return 100 + angle_in_corridor_right;
	}
	else if(right_front > SIDE_SENSOR_OPEN_LIMIT || right_back > SIDE_SENSOR_OPEN_LIMIT)
	{
		return 100 + angle_in_corridor_left;
	}
	else
	{
		return 100 + ((angle_in_corridor_right + angle_in_corridor_left) / 2);
	}
}

//Funktion som beräknar avvikelse från mitten i korridoren
/*
uint8_t Calculate_diff_from_middle_corridor(int8_t sensor_data_angle_corridor, uint8_t left_front, uint8_t left_back, uint8_t right_front, uint8_t right_back)
{
	int8_t little_add_on_right = HALF_ROBOT_LENGTH - tan(sensor_data_angle_corridor * 3.14 / 180.0f) * (SIDE_IR_DISTANCE / 2);
	int8_t little_add_on_left = HALF_ROBOT_LENGTH + tan(sensor_data_angle_corridor * 3.14 / 180.0f) * (SIDE_IR_DISTANCE / 2);
	
	uint16_t diff_from_right_wall = (little_add_on_right + right_back) * cos(sensor_data_angle_corridor * 3.14 / 180.0f);
	uint16_t diff_from_left_wall = (little_add_on_left + left_back) * cos(sensor_data_angle_corridor * 3.14 / 180.0f);
	
	if((left_back > SIDE_SENSOR_OPEN_LIMIT || left_front > SIDE_SENSOR_OPEN_LIMIT) && (right_front > SIDE_SENSOR_OPEN_LIMIT || right_back > SIDE_SENSOR_OPEN_LIMIT))
	{
		return 100;
	}
	else if(left_front > SIDE_SENSOR_OPEN_LIMIT || left_back > SIDE_SENSOR_OPEN_LIMIT)
	{
		return diff_from_right_wall - 100;
	}
	else if(right_front > SIDE_SENSOR_OPEN_LIMIT || right_back > SIDE_SENSOR_OPEN_LIMIT)
	{
		return 300 - diff_from_left_wall;
	}
	else
	{
		return (diff_from_right_wall - diff_from_left_wall) / 2 + 100;
	}
}*/

uint8_t Calculate_distance_from_right_wall(uint8_t left_front, uint8_t left_back, uint8_t right_front, uint8_t right_back)
{
	uint8_t diff_from_right_wall = (right_back + right_front) / 2;
	uint8_t diff_from_left_wall = (left_back + left_front) / 2;
	
	if((left_back > SIDE_SENSOR_OPEN_LIMIT || left_front > SIDE_SENSOR_OPEN_LIMIT) && (right_front > SIDE_SENSOR_OPEN_LIMIT || right_back > SIDE_SENSOR_OPEN_LIMIT))
	{
		return 100;
	}
	else if(left_front > SIDE_SENSOR_OPEN_LIMIT || left_back > SIDE_SENSOR_OPEN_LIMIT)
	{
		return diff_from_right_wall;
	}
	else if(right_front > SIDE_SENSOR_OPEN_LIMIT || right_back > SIDE_SENSOR_OPEN_LIMIT)
	{
		return 200 - diff_from_left_wall;
	}
	else
	{
		return ((diff_from_right_wall - diff_from_left_wall) / 2) + 100;
	}
}



//Funktioner som omvandlar sensor utdata till avstånd i mm alt cm
uint8_t S1_convert_sensor_value_left_front(uint8_t digital_distance)
{
	uint8_t mm_value;
	if (digital_distance >= 142) //Ger ut avståndet 20mm för alla digital_distance över 142
	{
		mm_value =20;
	}
	if (digital_distance <= 142 && digital_distance >= 103) //För digital_distance mellan 103-142 ges avstånd 40-60mm
	{
		mm_value = (((digital_distance - 220)*100) / -195);
	}
	else if (digital_distance <=103 && digital_distance >= 80) //För digital_distance mellan 80-103 ges avstånd 60-80mm
	{
		mm_value = (((digital_distance - 172)*100) / -115);
	}
	else if (digital_distance <= 80 && digital_distance >= 65) //För digital_distance mellan 80-65 ges avstånd 80-100mm
	{
		mm_value = (((digital_distance - 140)*100) / -75);
	}
	else if (digital_distance <= 65 && digital_distance >= 55) //För digital_distance mellan 55-65 ges avstånd 100-120mm
	{
		mm_value = (((digital_distance - 115)*10) / -5);
	}
	else if (digital_distance <= 55 && digital_distance >= 47) //För digital_distance mellan 47-55 ges avstånd 120-140mm
	{
		mm_value = (((digital_distance - 103)*10) / -4);
	}
	else if (digital_distance <= 47 && digital_distance >= 41) //För digital_distance mellan 41-47 ges avstånd 140-160mm
	{
		mm_value = (((digital_distance - 89)*10) / -3);
	}
	else if (digital_distance <= 41 && digital_distance >= 35) //För digital_distance mellan 35-41 ges avstånd 160-180mm
	{
		mm_value = (((digital_distance - 89)*10) / -3);
	}
	else if (digital_distance <= 35 && digital_distance >= 31) //För digital_distance mellan 31-35 ges avstånd 180-220mm
	{
		mm_value = (((digital_distance - 53)*10) / -1);
	}
	else //Ger ut avståndet 255mm för alla digit_distance under 31
	{
		mm_value = 255;
	}
	return mm_value;
}

uint8_t S2_convert_sensor_value_left_back(uint8_t digital_distance)
{
	uint8_t mm_value;
	
	if (digital_distance >= 144) //Ger ut avståndet 20mm för alla digital_distance över 144
	{
		mm_value =20;
	}
	if (digital_distance <= 144 && digital_distance >= 104) //För digital_distance mellan 104-144 ges avstånd 40-60mm
	{
		mm_value = ((digital_distance - 224) / -2);
	}
	else if (digital_distance <=104 && digital_distance >= 80) //För digital_distance mellan 80-104 ges avstånd 60-80mm
	{
		mm_value = (((digital_distance - 176)*10) / -12);
	}
	else if (digital_distance <= 80 && digital_distance >= 66) //För digital_distance mellan 80-66 ges avstånd 80-100mm
	{
		mm_value = ((digital_distance - 136)*10/ -7);
	}
	else if (digital_distance <= 66 && digital_distance >= 55) //För digital_distance mellan 55-66 ges avstånd 100-120mm
	{
		mm_value = (((digital_distance - 121)*100) / -55);
	}
	else if (digital_distance <= 55 && digital_distance >= 47) //För digital_distance mellan 47-55 ges avstånd 120-140mm
	{
		mm_value = (((digital_distance - 103)*10) / -4);
	}
	else if (digital_distance <= 47 && digital_distance >= 43) //För digital_distance mellan 43-47 ges avstånd 140-160mm
	{
		mm_value = (((digital_distance - 75)*10) / -2);
	}
	else if (digital_distance <= 43 && digital_distance >= 37) //För digital_distance mellan 37-42 ges avstånd 160-180mm
	{
		mm_value = (((digital_distance - 91)*10) / -3);
	}
	else if (digital_distance <= 37 && digital_distance >= 30) //För digital_distance mellan 30-37 ges avstånd 180-220mm
	{
		mm_value = (((digital_distance - 68)*100) / -17);
	}
	else //Ger ut avståndet 255mm för alla digit_distance under 30
	{
		mm_value = 255;
	}
	return mm_value;
}

uint8_t S3_convert_sensor_value_right_front(uint8_t digital_distance)
{
	uint8_t mm_value;
	
	if (digital_distance >= 150) //Ger ut avståndet 20mm för alla digital_distance över 150
	{
		mm_value =20;
	}
	if (digital_distance <= 150 && digital_distance >= 108) //För digital_distance mellan 108-150 ges avstånd 40-60mm
	{
		mm_value = (((digital_distance - 234)*10) / -21);
	}
	else if (digital_distance <=108 && digital_distance >= 83) //För digital_distance mellan 83-108 ges avstånd 60-80mm
	{
		mm_value = (((digital_distance - 183)*100) / -125);
	}
	else if (digital_distance <= 83 && digital_distance >= 68) //För digital_distance mellan 68-83 ges avstånd 80-100mm
	{
		mm_value = (((digital_distance - 143)*100) / -75);
	}
	else if (digital_distance <= 68 && digital_distance >= 56) //För digital_distance mellan 56-68 ges avstånd 100-120mm
	{
		mm_value = (((digital_distance - 128)*10) / -6);
	}
	else if (digital_distance <= 56 && digital_distance >= 41) //För digital_distance mellan 41-56 ges avstånd 120-160mm
	{
		mm_value = (((digital_distance - 101)*100) / -38);
	}
	else if (digital_distance <=41 && digital_distance >= 30) //För digital_distance mellan 30-41 ges avstånd 160-222mm
	{
		mm_value = (((digital_distance - 70)*100) / -18);
	}
	else //Ger ut avståndet 255mm för alla digit_distance under 30
	{
		mm_value = 255;
	}
	return mm_value;
}

uint8_t S4_convert_sensor_value_right_back(uint8_t digital_distance)
{
	int mm_value;
	
	if (digital_distance >= 155) //Ger ut avståndet 20mm för alla digit_distance över 144
	{
		mm_value =20;
	}
	if (digital_distance <= 155 && digital_distance >= 110) //För digital_distance mellan 110-152 ges avstånd 40-60mm
	{
		mm_value = (((digital_distance - 245)*100) / -225);
	}
	else if (digital_distance <=110 && digital_distance >= 84) //För digital_distance mellan 84-110 ges avstånd 60-80mm
	{
		mm_value = (((digital_distance - 188)*10) / -13);
	}
	else if (digital_distance <= 84 && digital_distance >= 68) //För digital_distance mellan 68-84 ges avstånd 80-100mm
	{
		mm_value = (((digital_distance - 148)*10) / -8);
	}
	else if (digital_distance <= 68 && digital_distance >= 56) //För digital_distance mellan 56-68 ges avstånd 100-120mm
	{
		mm_value = (((digital_distance - 128)*10) / -6);
	}
	else if (digital_distance <= 56 && digital_distance >= 48) //För digital_distance mellan 48-56 ges avstånd 120-140mm
	{
		mm_value = (((digital_distance - 104)*10) / -4);
	}
	else if (digital_distance <= 48 && digital_distance >= 40) //För digital_distance mellan 40-48 ges avstånd 140-160mm
	{
		mm_value = (((digital_distance - 104)*10) / -4);
	}
	else if (digital_distance <= 40 && digital_distance >= 29) //För digital_distance mellan 29-40 ges avstånd 160-233mm
	{
		mm_value = (((digital_distance - 64)*100) / -15);
	}
	else //Ger ut avståndet 255mm för alla digit_distance under 29
	{
		mm_value = 255;
	}
	return mm_value;
}

uint8_t S5_convert_sensor_value_front_long(uint8_t digital_distance)
{
	int cm_value;
	
	if (digital_distance >= 130) //Ger ut avståndet 5cm för alla digital_distance över 130
	{
		cm_value = 5;
	}
	if (digital_distance <= 130 && digital_distance >= 92) //För digital_distance mellan 92-130 ges avstånd 10-15cm
	{
		cm_value = (((digital_distance-206)*10) / -76);
	}
	else if (digital_distance <= 92 && digital_distance >= 72) //För digital_distance mellan 72-92 ges avstånd 10-15cm
	{
		cm_value = ((digital_distance-152) / -4);
	}
	else if (digital_distance <= 72 && digital_distance >= 60) //För digital_distance mellan 60-72 ges avstånd 15-20cm
	{
		cm_value = (((digital_distance-117)*10) / -24);
	}
	else if (digital_distance <= 60 && digital_distance >= 53) //För digital_distance mellan 53-60 ges avstånd 20-30cm
	{
		cm_value = (((digital_distance-95)*10) / -14);
	}
	else if (digital_distance <= 53 && digital_distance >= 35) //För digital_distance mellan 35-53 ges avstånd 30-50cm
	{
		cm_value = (((digital_distance-80)*10) / -9);
	}
	else if (digital_distance <= 35 && digital_distance >= 30) //För digital_distance mellan 30-35 ges avstånd 50-67cm
	{
		cm_value = (((digital_distance-50)*10) / -3);
	}
	else //Ger ut avståndet 255cm för alla digit_distance under 30
	{
		cm_value = 255;
	}
	return cm_value;
}
