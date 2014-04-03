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

//Adressbytes:
#define ADDRESS_BYTE_SENSOR_DATA 0x01
#define ADDRESS_BYTE_GIVE_DISTANCE 0x02
#define ADDRESS_BYTE_DISTANCE 0x03
#define ADDRESS_BYTE_START_CALC_ANGLE 0x04
#define ADDRESS_BYTE_GIVE_ANGLE 0x05
#define ADDRESS_BYTE_ANGLE 0x06
#define ADDRESS_BYTE_GUIDED_DECISIONS 0x07
#define ADDRESS_BYTE_AUTO_DECISIONS 0x08

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