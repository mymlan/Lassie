/*
 * tempkarta.c
 *
 * Created: 4/14/2014 4:22:05 PM
 *  Author: henba136
 */ 


#include <avr/io.h>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

//init
uint8_t sensor1 = 1; // 255 -> ingen vägg
uint8_t sensor2 = 1;
uint8_t sensor3 = 1;
uint8_t sensor4 = 1;
uint8_t sensor5 = 1;
uint8_t vinkelsensor = 1;
int robot_dir = 0; // nord, öst, syd, väst

//vill skriva funktion som tar in riktning och sensorvärden för att bestämma vilka riktningar/väderstreck som är öppna
//returnerar ett tal på 4 bitar i ordningen nord,öst,syd,väst -> 1111, rdir är robtens riktning 0-3 i ordningen nord,öst,syd,väst
//jag vet inte om det finns snyggare samband, men jag går igenom alla fall till att börja med
int what_is_open(int left, int right, int front, int rdir)
{
	if(rdir == 0) // om vi åker norrut
	{
		int buffer = 2; // alltid öppet bakåt
		if(front > 250)
		{
			buffer = buffer + 8;
		}
		if(right > 250)
		{
			buffer = buffer + 4;
		}
		if(left > 250)
		{
			buffer = buffer + 1;
		}
		return buffer;
	}
	else-if(rdir == 1)//om vi åker österut
	{
		int buffer = 1; // alltid öppet bakåt
		if(left > 250)
		{
			buffer = buffer + 8;
		}
		if(front > 250)
		{
			buffer = buffer + 4;
		}
		if(right > 250)
		{
			buffer = buffer + 2;
		}
		return buffer;		
	}
	else-if(rdir == 2)//om vi åker söderut
	{
		int buffer = 8; // alltid öppet bakåt
		if(left > 250)
		{
			buffer = buffer + 4;
		}
		if(front > 250)
		{
			buffer = buffer + 2;
		}
		if(left > 250)
		{
			buffer = buffer + 1;
		}
		return buffer;
	}
	
	else-if(rdir == 3)//om vi åker västerut
	{
		int buffer = 4; // alltid öppet bakåt
		if(right > 250)
		{
			buffer = buffer + 8;
		}
		if(left > 250)
		{
			buffer = buffer + 2;
		}
		if(front > 250)
		{
			buffer = buffer + 1;
		}
		return buffer;
	}
		return 0; //något har gått fel
}

//------ C, VI LOVAR ATT DESSA STRUCTAR DEFINIERAS -------//
struct node_;
struct link_;

//-------------------- STRUCT-DEFINITIONER------------------------//
typedef struct link_
{
	int length;
	struct node_ *p_node;
	char dir;
} link;
typedef struct node_
{
	int x;
	int y;
	link links[];
} node;

//------ C, VI LOVAR ATT DESSA FUNKTIONER DEFINIERAS, AKA INITIERING -------//
node* Newnode(int, int, int);
link* Newlink(int, int, node*);

//-------------------- NEW STRUCT DEFINITIONER------------------------//
link* Newlink(int length_in, int dir_in, node* nodeptr)
{
	link *p_lik = malloc(sizeof(link));
	
	p_lik->length = length_in;
	p_lik->dir = dir_in;
	p_lik->p_node = nodeptr;
	
	return p_lik;
}

node* Newnode(int x_in, int y_in, int number_of_roads)
{
	node *p_nod = malloc(sizeof(node));
	
	p_nod->x = x_in;
	p_nod->y = y_in;
	//for så många ööpningar, sen placera i array också, kanske if öppen på alla öppningar och directions, case i en case???
	
	node *ps;
	ps = malloc(2 * sizeof(int) + sizeof(link) + number_of_roads * sizeof(node)); // Vad är det här??
	
	ps->x = 32;
	ps->y = 45;
	ps->links[0].length = 2;
	ps->links[1].length = 2;
	ps->links[2].length = 2;
	ps->links[3].length = 2; // Fattar inte varför även denna fungerar, för att structen är []
	ps->links[4].length = 2; // Fattar inte varför även denna fungerar
	ps->links[5].length = 2; // Fattar inte varför även denna fungerar
	ps->links[6].length = 2; // Fattar inte varför även denna fungerar
	ps->links[7].length = 2; // Fattar inte varför även denna fungerar
	p_nod->links[45].length = 54;
	/*
	struct link array[2];
	p_nod->links = array;
	p_nod->links[0] = *Newlink(1,1,NULL);
	p_nod->links[1] = *Newlink(1,1,NULL);
	p_nod->links[2] = *Newlink(1,1,NULL);
	*/
	return p_nod;
}

//--------------------PUSH ARRAY------------------------//
/*
struct link Push_array(struct link array[])
{
	struct link temp[];
	return *temp;
}
*/




int Map_main(void)
{
	//R24 = 0x07;
	// Skapa origo-nod
	node* p_node1 = Newnode(0, 0, 2);
	p_node1->links[0].dir = 0; // finns(inte = 0 ??) väg norr
	p_node1->links[1].dir = 1; // finns väg öster
	node* robot_node = p_node1;
	
	// Kommer till en nod, tror bra att skriva om till funktion senare när klar
	node* p_node2 = Newnode(0, 4, 1);
	if (robot_node->links[0].dir == robot_dir) // Leta upp den link som har riktning i robotens körriktning
	{
		robot_node->links[0].p_node = p_node2; // Uppdatera nod-pekare för gammal nod
		robot_node->links[0].length = 4; // Uppdatera båglängd för gammal nod
	}
	// else if ...
		
	
	// Dummykod så noderna används
	node node2;
	node2.x = p_node1->x - 9;
	p_node1->y = node2.y + 5;
	
	/*
	struct node node1;
	struct node node2;
	node1.x = 35;
	struct link link1;
	struct link link2;
	link1.length = 42;
	link1.p_node = &node2;
	link1.p_node->x = 32;
	link2.p_node = &node1;
	link2.p_node->y = 43;
	node1.links[1] = link1;
	node1.links[1].p_node-> x = 78;
	//node1.array_of_links[1] = link1;
	//node1.array_of_links[1].p_node = &node2;
	*/
    while(1)
    {
        //node1.x = 64;
		//TODO:: Please write your application code 
    }
	return 0;
}


//funktion hitta mål

//funktion hitta närmsta väg(nod1 nod2)

//funktion kör x meter

//funktion fram x korsningar

//funktion innan en korsning(2 sensorer)?

//funktion i en korning(4 sensorer)?


//funktion bästa väg hittad?

//funktion åk väg(nod1 nod2)

//funktion skicka kommando

//funktion utför rotation
