
#define COMMON_SET_PIN(PORT, PIN) (PORT |= (1 << PIN))
// vänterskift med en etta, sedan or med register

#define COMMON_CLEAR_PIN(PORT, PIN) (PORT &= (~(1 << PIN)))
// and med ettor och en nolla, på rätt plats

#define COMMON_SET_BIT(REGISTER, BIT) (REGISTER |= (1 << BIT))
// vänterskift med en etta, sedan or med register

#define COMMON_CLEAR_BIT(REGISTER, BIT) (REGISTER &= (~(1 << BIT)))
// and med ettor och en nolla, på rätt plats

#define COMMON_TOGGLE_PIN(PORT, PIN) (PORT ^= (1 << PIN))

//ID-bytes:
#define ID_BYTE_IR_SENSOR_DATA (0x01)//7byte
#define ID_BYTE_DISTANCE (0x03)//1byte cm
#define ID_BYTE_ROTATION_FINISHED (0x06) //inget meddelande!
#define ID_BYTE_FOUND_RFID (0x0B)

#define ID_BYTE_MANUAL_DECISIONS (0x07)
#define ID_BYTE_AUTO_DECISIONS (0x08)

#define ID_BYTE_GIVE_DISTANCE (0x02)
#define ID_BYTE_START_ANGULAR_RATE_SENSOR (0x04)
#define ID_BYTE_GIVE_IR_SENSOR_DATA (0x09)

#define MASTER_RECIEVED_BYTE (0x0A)

//Kommando till styrmodulen

#define COMMAND_STOP (0x00)
#define COMMAND_FORWARD (0x01)
#define COMMAND_BACKWARD (0x02)
#define COMMAND_TURN_RIGHT (0x03)
#define COMMAND_TURN_LEFT (0x04)
#define COMMAND_ROTATE_RIGHT (0x05)
#define COMMAND_ROTATE_LEFT (0x06)
#define COMMAND_CLOSE_CLAW (0x07)
#define COMMAND_OPEN_CLAW (0x08)
#define COMMAND_FORWARD_NOT_REGULATED (0x09)
#define COMMAND_BACKWARD_NOT_REGULATED (0x0A)

#define NUMBER_OF_BYTES_IR_SENSOR_DATA (7)

//Sensor
#define ANGULAR_RATE_SENSITIVITY (1.28)
#define ANGULAR_RATE_OFFSET (133.1378)

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
#define COMMUNICATION_INFINITY (255)
#define TRAVELED_BLOCKS_MARGIN (20)
#define SIDE_SENSOR_OPEN_LIMIT (200)
#define SIZE_OF_SQUARE_IN_CM (40)

