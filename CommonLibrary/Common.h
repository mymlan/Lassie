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

#define START_CALC_ANGLE 0x04
