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
uint8_t sensor1 = 1;
uint8_t sensor2 = 1;
uint8_t sensor3 = 1;
uint8_t sensor4 = 1;
uint8_t sensor5 = 1;
uint8_t vinkelsensor = 1;

//------ C, VI LOVAR ATT DESSA STRUCTAR DEFINIERAS -------//
struct node_;
struct link_;

//-------------------- STRUCT DEFINITIONER------------------------//
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

//------ C, VI LOVAR ATT DESSA FUNKTIONER DEFINIERAS -------//
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
	ps = malloc(2 * sizeof(int) + sizeof(link) + number_of_roads * sizeof(node));
	
	ps->x = 32;
	ps->y = 45;
	ps->links[0].length = 2;
	ps->links[1].length = 2;
	ps->links[2].length = 2;
	ps->links[3].length = 2; // Fattar inte varför även denna fungerar
	ps->links[4].length = 2; // Fattar inte varför även denna fungerar
	ps->links[5].length = 2; // Fattar inte varför även denna fungerar
	ps->links[6].length = 2; // Fattar inte varför även denna fungerar
	ps->links[7].length = 2; // Fattar inte varför även denna fungerar
	
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
	node* p_node = Newnode(0, 0, 2);
	p_node->links[0].dir = 0; // finns väg norr
	p_node->links[1].dir = 1; // finns väg öster
	node* robot_node = p_node;
	int robot_dir = 0;
	
	// Kommer till en nod
	node* p_node2 = Newnode(0, 4, 1);
	if (robot_node->links[0].dir == robot_dir) // Leta upp den link som har riktning i robotens körriktning
	{
		robot_node->links[0].p_node = p_node2; // Uppdatera nod-pekare för gammal nod
		robot_node->links[0].length = 4; // Uppdatera båglängd för gammal nod
	}
	// else if ...
		
	
	// Dummykod så noderna används
	node node2;
	node2.x = p_node->x - 9;
	p_node->y = node2.y + 5;
	
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
