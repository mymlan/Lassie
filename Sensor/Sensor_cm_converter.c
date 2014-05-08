#include <stdint.h>
#include <avr/interrupt.h>
#include <math.h>
#include "Sensor_SPI.h"
#include "Sensor_cm_converter.h"
#include "../CommonLibrary/Common.h"
#define Baudrate 2400  //Bits per second hos RFID
#define F_CPU 18432000UL  //Clock freq. hos MC
#define Baud_Prescale ((F_CPU/(Baudrate*16UL))-1)  //Baud rate generator (prescaler) f�r USART
volatile uint8_t RFID_tag_read[10];  //Array med 10 bytes d�r RFID ID lagras
volatile uint8_t RFID_tag_correct[10];  //Avl�st ID p� RFID l�ggs in h�r om korrekt avl�sning skett
volatile uint8_t RFID_count = 0;  //R�knare f�r att kontrollera att r�tt antal bytes l�sts in fr�n RFID
volatile uint8_t RFID_start_read = 0;  //Kontrollerar startbit innan bytes l�gg in p� RFID_tag_read

volatile unsigned char test;
//volatile uint8_t  sensor1, sensor2, sensor3, sensor4, sensor5;

static volatile int diff_from_middle_corridor; // Fick det inte att fungera med uint8_t
static volatile uint8_t angle_corridor;

static volatile uint8_t angular_rate_value;
static volatile float angular_rate_total = 0;
static volatile float angular_rate_diff;

void init_interrupts()
{
	ACSR = (1>>ACD)|(1>>ACBG)|(1<<ACIE)|(1>>ACIC)|(0>>ACIS0)|(0>>ACIS1);  //ACD=0 AC p�, ACBG=0 external comparator pin AIN0, ACIE=1 interrupt enable, ACIC=0 Timer/Counter disabled, ACIS0..1=1 interrupt on rising/falling output edge
	PORTD |= 1<<PORTD6;  //S�tter PORTD6 (pinne 20) till HIGH som anv�nds till sp�nningsdelaren hos reflexsensorn
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
	//eventuellt prescale r�kning p� buad?!?!?!
	UBRR0H = (unsigned char)(Baud_Prescale>>8);//set upper 8 bit of baudrate
	UBRR0L = (unsigned char)Baud_Prescale; //set rest of bits of baudrate
}

void init_variable()
{
	reflex_count = 0;
	next_sensor_to_be_converted = IR_LEFT_FRONT;
}

ISR(USART0_RX_vect)
{	
	if (RFID_count==10)  //kollar om korrekt startbit
	{
		if (UDR0 == 13)  //kollar om korrekt stopbit
		{
			for (int i = 0; i <= 9; i++)
			{
				RFID_tag_correct[i] = RFID_tag_read[i];  //Laddar �ver korrekt avl�st RFID ID
			}
			//Ifall vi vill kunna identifiera en specifik RFID s� ska vi kolla igenom den avl�sta RFIDn med ett antal k�nda RFID-nummer
			/*if (RFID_tag_correct[9] == 68) 
			{
				SPI_sensor_send_data_byte(ID_BYTE_FOUND_RFID, RFID_1);
			}
			*/
			SPI_sensor_send_data_byte(ID_BYTE_FOUND_RFID, 1);  //Meddelar att RFID hittats (samt vilken RFID som hittats)
			RFID_count = 0; 
			RFID_start_read = 0;
		}
		else
		{
			RFID_count = 0;
		}
	}
	//L�gger in avl�sta bytes om korrekt startbit hittats
 	if (RFID_start_read == 1)  
	{
		RFID_tag_read[RFID_count] = UDR0;
		RFID_count++;
	}
	//Meddelar resten av ISR att en korrekt startbit hittats
	if (UDR0 == 10)
	{
		RFID_start_read = 1;
	}
}

ISR (ADC_vect)
{ 
	switch (next_sensor_to_be_converted)
	{
		
		case(IR_LEFT_FRONT):
		ir_sensor_data[0] = S1_convert_sensor_value_left_front(ADCH);
		//sensor1 = S1_convert_sensor_value_left_front(ADCH); //sensor1 f�r det AD-omvandlade v�rdet
		next_sensor_to_be_converted = IR_LEFT_BACK; //adderar 1 till sensor_which_AD_converted
		ADMUX = (1<<ADLAR)|(1<<REFS0)|(1<<MUX2); //s�tter ADMUX till PA4 f�r att AD-omvandla left_back IR-sensor
		break;
		
		case(IR_LEFT_BACK):
		ir_sensor_data[1] = S2_convert_sensor_value__left_back(ADCH);
		//sensor2 = S2_convert_sensor_value__left_back(ADCH); //sensor2 f�r det AD-omvandlade v�rdet
		next_sensor_to_be_converted = IR_RIGHT_FRONT; //adderar 1 till sensor_which_AD_converted
		ADMUX = (1<<ADLAR)|(1<<REFS0)|(1<<MUX2)|(1<<MUX0); //S�tter ADMUX till PA5 f�r att AD-omvandla right_front IR-sensor
		break;
		
		case(IR_RIGHT_FRONT):
		ir_sensor_data[2] = S3_convert_sensor_value_right_front(ADCH);
		//sensor3 = S3_convert_sensor_value_right_front(ADCH); //sensor3 f�r det AD-omvandlade v�rdet
		next_sensor_to_be_converted = IR_RIGHT_BACK; //adderar 1 till sensor_which_AD_converted
		ADMUX = (1<<ADLAR)|(1<<REFS0)|(1<<MUX2)|(1<<MUX1); //S�tter ADMUX till PA6 f�r att AD-omvandla right_back IR-sensor
		break;
		
		case(IR_RIGHT_BACK):
		ir_sensor_data[3] = S4_convert_sensor_value_right_back(ADCH);
		//sensor4 = S4_convert_sensor_value_right_back(ADCH); //sensor4 f�r det AD-omvandlade v�rdet
		next_sensor_to_be_converted = IR_FRONT_LONG; //adderar 1 till sensor_which_AD_converted
		ADMUX = (1<<ADLAR)|(1<<REFS0)|(1<<MUX2)|(1<<MUX1)|(1<<MUX0); //S�tter ADMUX till PA7 f�r att AD-omvandla front_long IR-sensor
		break;
		
		case(IR_FRONT_LONG):
		ir_sensor_data[4] = S5_convert_sensor_value_front_long(ADCH);
		//sensor5 = S5_convert_sensor_value_front_long(ADCH); //sensor5 f�r det AD-omvandlade v�rdet
		next_sensor_to_be_converted = 0; //nollst�ller sensor_which_AD_converted
		ADMUX = (1<<ADLAR)|(1<<REFS0)|(1<<MUX1)|(1<<MUX0); //S�tter ADMUX till PA3 f�r att b�rja om och AD-omvandla left_front IR-sensor
		angle_corridor = calculate_angle_corridor(ir_sensor_data[0], ir_sensor_data[1], ir_sensor_data[2], ir_sensor_data[3]); //Ber�knar vinkeln roboten har i en korridor utifr�n centrumlinjen
		diff_from_middle_corridor = calculate_diff_from_middle_corridor(angle_corridor - 90, ir_sensor_data[1], ir_sensor_data[3]); //Ber�knar avvikelsen fr�n mitten av fr�n 20cm vilket �r mitten av korridoren
		if (diff_from_middle_corridor > 255)
		{
			diff_from_middle_corridor = 255;
		}
		ir_sensor_data[5] = angle_corridor; //l�gger in vinkel i korridoren p� plats 6 i ir_sensor_data;
		ir_sensor_data[6] = diff_from_middle_corridor;//l�gger in avvikelsen p� plats 7 i ir_sensor_data;
		break;
		
		case(ANGULAR_RATE):
		if ((-27.5 < angular_rate_total) & (angular_rate_total < 27.5)) //kollar om roboten roterat 90 grader i n�gon riktning
		{
			angular_rate_value = ADCH; //l�ser av vinkelhastigheten
			angular_rate_diff = (angular_rate_value - ANGULAR_RATE_OFFSET)*ANGULAR_RATE_SENSITIVITY; //ber�knar f�r�ndringen av roterad vinkel sedan sedan f�rra avl�sningen
			angular_rate_total += (angular_rate_diff / 10000); //Addera f�r�ndringen av roterad vinkel till den totala roterade vinkeln
			//ADMUX = (1<<ADLAR)|(1<<REFS0)|(1<<MUX1); // S�tter ADMUX till PA2 f�r  avl�sning av vinkelhastighetssensorn
		} else
		{
			ADMUX = (1<<ADLAR)|(1<<REFS0)|(1<<MUX1)|(1<<MUX0); //S�tter ADMUX till PA3 s� att IR v�nster fram b�rjar AD-omvandlas
			angular_rate_total = 0; //s�tter roterad vinkel till 0 n�r roboten roterat 90 grader i n�gon riktning
			next_sensor_to_be_converted = IR_LEFT_FRONT; //s�tter count till 0 f�r att �terg� till AD-omvandling av IR-sensorerna
			reflex_count = 0; //nollst�ller avl�sningen av avst�ndet f�r att kunna p�b�rja ny avl�sning
			SPI_sensor_send_rotation_finished(); //skickar meddelande till KOM att rotationen �r klar
			ACSR |= (1>>ACD); //Startar Analog Comparator (reflexsensorn)
		}
		
		break;
	}
	
	ADCSRA |= (1<<ADSC); //startar ny AD-omvandling
	
	
}

ISR(ANALOG_COMP_vect){
	//Tr�skelv�rdet h�js resp. s�nks om interruptet startars p� l�g resp. h�g 
	if (PORTD & (1<<PORTD6)){ 
		PORTD |= (1>>PORTD6);  
	}
	else{
		PORTD |= (1<<PORTD6);  
	}
	reflex_count = reflex_count+1;  //R�knar upp 
	ACSR |= (1<<ACI);  //Tar bort eventuella interrupts p� k� (endast ett ska r�knas varje g�ng)
}

//Funktioner som omvandlar sensor utdata till avst�nd i mm
uint8_t S1_convert_sensor_value_left_front(uint8_t digital_distance)
{
	uint8_t mm_value;
	
	if (digital_distance >= 144)
	{
		mm_value =20;
	}
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
	if (digital_distance >= 142)
	{
		mm_value =20;
	}
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
	
	if (digital_distance >= 150)
	{
		mm_value =20;
	}
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
	
	if (digital_distance >= 155)
	{
		mm_value =20;
	}
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

//Funktion som ber�knar vinkeln roboten har i en korridor

uint8_t calculate_angle_corridor(uint8_t left_front, uint8_t left_back, uint8_t right_front, uint8_t right_back)
{
	int8_t angle_in_corridor_right;
	int8_t angle_in_corridor_left;
	
	angle_in_corridor_right = atan2(right_back - right_front, SIDE_IR_DISTANCE) * 180 / 3.14;
	angle_in_corridor_left = atan2(left_front - left_back, SIDE_IR_DISTANCE) * 180 / 3.14;
	
	return 90 + angle_in_corridor_left;
	return 90 + angle_in_corridor_right;
	
	return 90 + (angle_in_corridor_right + angle_in_corridor_left) / 2;
}

//Funktion som ber�knar avvikelse fr�n mitten i korridoren

uint8_t calculate_diff_from_middle_corridor(int8_t angle_corridor, uint8_t left_back, uint8_t right_back)
{
	int8_t little_add_on_right = HALF_ROBOT_LENGTH - tan(angle_corridor * 3.14 / 180.0f) * (SIDE_IR_DISTANCE / 2);
	int8_t little_add_on_left = HALF_ROBOT_LENGTH + tan(angle_corridor * 3.14 / 180.0f) * (SIDE_IR_DISTANCE / 2);
	
	uint16_t diff_from_right_wall = (little_add_on_right + right_back) * cos(angle_corridor * 3.14 / 180.0f);
	uint16_t diff_from_left_wall = (little_add_on_left + left_back) * cos(angle_corridor * 3.14 / 180.0f);
	
	return diff_from_left_wall - 100;
	return diff_from_right_wall - 100;
	return (diff_from_right_wall - diff_from_left_wall) / 2 + 100;
}