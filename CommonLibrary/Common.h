/*
 * CommonLibrary.c
 *
 * Created: 2014-03-29 14:38:18
 *  Author: Mymlan
 */ 

//Exempel på macro

#define COMMON_SET_PIN(PORT, PIN) (PORT |= (1 << PIN))
// vänterskift med en etta, sendan or med register

#define COMMON_CLEAR_PIN(PORT, PIN) (PORT &= (~(1 << PIN)))
// and med ettor och en nolla, på rätt plats

#define COMMON_SET_BIT(REGISTER, BIT) (REGISTER |= (1 << BIT))
// vänterskift med en etta, sendan or med register

#define COMMON_CLEAR_BIT(REGISTER, BIT) (REGISTER &= (~(1 << BIT)))
// and med ettor och en nolla, på rätt plats

#define COMMON_TOGGLE_PIN(PORT, PIN) (PORT ^= (1 << PIN))

//ID-bytes:
#define ID_BYTE_SENSOR_DATA 0x01
#define ID_BYTE_GIVE_DISTANCE 0x02
#define ID_BYTE_DISTANCE 0x03
#define ID_BYTE_START_CALC_ANGLE 0x04
#define ID_BYTE_GIVE_ANGLE 0x05
#define ID_BYTE_ANGLE 0x06
#define ID_BYTE_GUIDED_DECISIONS 0x07
#define ID_BYTE_AUTO_DECISIONS 0x08
#define ID_BYTE_GIVE_SENSOR_DATA 0x09

#define MASTER_RECIEVED_BYTE 0x0A

//Kommando till styrmodulen
#define COMMAND_STOP 0x00
#define COMMAND_FORWARD 0x01 
#define COMMAND_BACKWARD 0x02
#define COMMAND_TURN_RIGHT 0x03
#define COMMAND_TURN_LEFT 0x04
#define COMMAND_ROTATE_RIGHT 0x05
#define COMMAND_ROTATE_LEFT 0x06
#define COMMAND_CLOSE_CLAW 0x07
#define COMMAND_OPEN_CLAW 0x08

/*
uint8_t IR_RIGHT_FRONT = 0;
uint8_t IR_RIGHT_BACK = 0;
uint8_t IR_LEFT_FRONT = 0;
uint8_t IR_LEFT_BACK = 0;
uint8_t IR_INFRONT = 0;
uint8_t IR_ANGLE = 0;
uint8_t COMMON_IR_SENSOR_DATA[6] = {IR_RIGHT_FRONT, IR_RIGHT_BACK, IR_LEFT_FRONT, IR_LEFT_BACK, IR_INFRONT, IR_ANGLE};
	*/

#define NORTH (0)
#define EAST (1)
#define SOUTH (2)
#define WEST (3)
#define TRUE (1)
#define FALSE (0)
#define ERROR_IN_GET_DIJKPOINTERS_CARDINAL_DIRECTION (5)
#define ERROR_IN_WHAT_IS_OPEN (17)
#define NUMBER_OF_LINKS (4)
#define ERROR_IN_FIND_INDEX_OF_NODE (201)
#define INF (255)
#define TRAVELED_BLOCKS_MARGIN (20)
#define SIDE_SENSOR_OPEN_LIMIT (200)
#define SIZE_OF_SQUARE_IN_CM (40)