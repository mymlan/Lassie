/*
 * Common.h
 * F�rfattare: denbe220, henba136, mikiv293, linka950, hugjo456, gusno119
 * 2014-06-05
 * version 1.0
 */ 

#define COMMON_SET_PIN(PORT, PIN) (PORT |= (1 << PIN))
// v�nterskift med en etta, sedan or med register

#define COMMON_CLEAR_PIN(PORT, PIN) (PORT &= (~(1 << PIN)))
// and med ettor och en nolla, p� r�tt plats

#define COMMON_SET_BIT(REGISTER, BIT) (REGISTER |= (1 << BIT))
// v�nterskift med en etta, sedan or med register

#define COMMON_CLEAR_BIT(REGISTER, BIT) (REGISTER &= (~(1 << BIT)))
// and med ettor och en nolla, p� r�tt plats

#define COMMON_TOGGLE_PIN(PORT, PIN) (PORT ^= (1 << PIN))

//ID-bytes (Switch-case)
#define ID_BYTE_IR_SENSOR_DATA (0x01)
#define ID_BYTE_DISTANCE (0x03)
#define ID_BYTE_ROTATION_FINISHED (0x06)
#define ID_BYTE_FOUND_RFID (0x0B)
#define ID_BYTE_REACHED_RFID (0x0F)

#define ID_BYTE_MANUAL_DECISIONS (0x07)
#define ID_BYTE_AUTO_DECISIONS (0x08)

#define ID_BYTE_GIVE_DISTANCE (0x02)
#define ID_BYTE_START_ANGULAR_RATE_SENSOR (0x04)
#define ID_BYTE_GIVE_IR_SENSOR_DATA (0x09)
#define ID_BYTE_COUNT_DOWN_REFLEX_SENSOR (0x0E)

#define ID_BYTE_LINK_COORDINATES (0x0C)
#define ID_BYTE_MAP_PARAMETERS (0x0D)

//Kommando till styrmodulen (Switch-case)
#define COMMAND_STOP (0x10)
#define COMMAND_FORWARD (0x11)
#define COMMAND_BACKWARD (0x12)
#define COMMAND_TURN_RIGHT (0x13)
#define COMMAND_TURN_LEFT (0x14)
#define COMMAND_ROTATE_RIGHT (0x15)
#define COMMAND_ROTATE_LEFT (0x16)
#define COMMAND_CLOSE_CLAW (0x17)
#define COMMAND_OPEN_CLAW (0x18)
#define COMMAND_FORWARD_NOT_REGULATED (0x19)
#define COMMAND_BACKWARD_NOT_REGULATED (0x1A)
#define COMMAND_TIGHT_TURN_RIGHT (0x1B)
#define COMMAND_TIGHT_TURN_LEFT (0x1C)

#define NUMBER_OF_BYTES_IR_SENSOR_DATA (7)
#define NUMBER_OF_SENSOR_DATA_TO_SKIP_TO_SEND_TO_PC (4)
#define MANUAL_DECISIONS_ACTIVATED (16)
#define TIMER_COMPARE_NUMBER (14400)

//Sensorer att omvandla i AD-omvandlaren (Swich-case)
#define IR_LEFT_FRONT (0x00)
#define IR_LEFT_BACK (0x01)
#define IR_RIGHT_FRONT (0x02)
#define IR_RIGHT_BACK (0x03)
#define IR_FRONT_LONG (0x04)
#define ANGULAR_RATE (0x05)

// RFID-taggar
#define RFID_1 (1)
#define RFID_2 (2)
#define RFID_3 (3)
#define RFID_4 (4)
#define RFID_5 (5)

//Styr
#define REGULATED_FORWARD (1)
#define REGULATED_BACKWARD (2)
#define NO_NEED_TO_REGULATE (0)

#define MAX_SPEED (65535)

//comm-karta
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
#define DIJKSTRA_INFINITY (255)
#define TRAVELED_BLOCKS_MARGIN (20)
#define SIDE_SENSOR_OPEN_LIMIT (220)
#define SIZE_OF_SQUARE_IN_CM (40)
#define HALF_SIZE_OF_SQUARE_IN_CM (20)
#define FRONT_SENSOR_LIMIT (20)
#define HALF_SIZE_OF_SQUARE_IN_TICS (8.21)
#define SIZE_OF_SQUARE_IN_TICS (16.43)

//test
#define COMMAND_STOP_1 (0x21)
#define COMMAND_STOP_2 (0x22)
#define COMMAND_STOP_3 (0x23)
#define COMMAND_STOP_4 (0x24)
#define COMMAND_STOP_5 (0x25)
#define COMMAND_STOP_6 (0x26)

//levels
#define FIRST_WAIT (0)
#define SEARCH_FOR_GOAL (1)
#define KEEP_SEARCHING (2)
#define RETURN_AFTER_FOUND (3)
#define WAIT_FOR_ITEM (4)
#define GRAB_ITEM_AND_CALC_DIJK (5)
#define GO_TO_GOAL (6)
#define DROP_ITEM (7)
#define RETURN_AFTER_DELIVERED (8)
#define FINISHED (9)
