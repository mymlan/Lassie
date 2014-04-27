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
#include <stdbool.h> // bool

//init
uint8_t sensor1 = 1; // 255 -> ingen vägg
uint8_t sensor2 = 1;
uint8_t sensor3 = 1;
uint8_t sensor4 = 1;
uint8_t sensor5 = 1;
uint8_t vinkelsensor = 1;
//int robot_dir = 0; // nord, öst, syd, väst

//------ C, VI LOVAR ATT DESSA STRUCTAR DEFINIERAS -------//
struct node_;
struct link_;

//-------------------- STRUCT-DEFINITIONER------------------------//
typedef struct link_
{
	int length;
	struct node_ *p_node;
	//char dir; // tas bort om vi kör på alltid 4 länkar
	bool open; // true om väg finns
} link;
typedef struct node_
{
	int x;
	int y;
	bool start; // sant/falskt
	bool goal; // sant/falskt
	link links[4]; // Jag tror det är lättare om den har fix storlek och så får vi ha en bool för varje link som avgör öppen eller ej
	// Det ska vara en fyra här (4 st element 0-3)
} node;

//------ C, VI LOVAR ATT DESSA FUNKTIONER DEFINIERAS, AKA INITIERING -------//
node* Newnode(int, int);
link* Newlink(int, node*);

//-------------------- NEW STRUCT DEFINITIONER------------------------//
link* Newlink(int length_in, node* nodeptr)
{
	link *p_lik = malloc(sizeof(link));
	
	p_lik->length = length_in;
	//p_lik->dir = dir_in;
	p_lik->p_node = nodeptr;
	
	return p_lik;
}

node* Newnode(int x_in, int y_in)
{
	node *p_node = malloc(sizeof(node)); // Allocerar minne
	
	//for så många ööpningar, sen placera i array också, kanske if öppen på alla öppningar och directions, case i en case???
	
	//node *ps;
	//node *p_node = malloc(2 * sizeof(int) + sizeof(link) + number_of_roads * sizeof(node)); // Vad är det här?? // Detta allokerar en viss mängd minne för structen så att arrayen kan anta "godtycklig" storlek
	
	p_node->x = x_in;
	p_node->y = y_in;
	
	p_node->start = false;
	p_node->goal = false;
	
	p_node->links[0].open = 0;
	p_node->links[1].open = 0;
	p_node->links[2].open = 0;
	p_node->links[3].open = 0;
	/*
	//link arra[];
	//p_node->links = arra;
	p_node->links[0] = Newlink(1,NULL);
	p_node->links[1] = Newlink(1,NULL);
	p_node->links[2] = Newlink(1,NULL);
	*/
	return p_node;
}

//--------------------PUSH ARRAY------------------------//
/* // Denna behövs inte om vi kör på fast storlekt på arrayen
struct link Push_array(struct link array[])
{
	struct link temp[];
	return *temp;
}
*/

//-------------------FUNKTIONER---------------------------//
//vill skriva funktion som tar in riktning och sensorvärden för att bestämma vilka riktningar/väderstreck som är öppna
//returnerar ett tal på 4 bitar i ordningen nord,öst,syd,väst -> 1111, rdir är robtens riktning 0-3 i ordningen nord,öst,syd,väst
//jag vet inte om det finns snyggare samband, men jag går igenom alla fall till att börja med
int What_is_open(int left, int right, int front, int rdir)
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
	else if(rdir == 1)//om vi åker österut
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
	else if(rdir == 2)//om vi åker söderut
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
	
	else if(rdir == 3)//om vi åker västerut
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
	return 0; //något har gått fel // man returnerar egentligen bara 0 när att gått bra. Andra siffror betyrer vissa felmeddelanden
}

// Create_node
// Skapar en ny nod kopplad till robotpekaren enligt givna argument. Uppdaterar p_robot_node så returvärde behövs inte.
void Create_node(int x, int y, node* p_robot_node, int robot_dir, int length, int open_walls)
{
	// GENERELLT ARGUMENT betyder att det ska vara i någon form av argument till funktionen. Just nu bara dummydata
		// 1.Skapa ny nod
	node* p_node = Newnode(x, y); // 1.
		// 2.Uppdatera gammal nod
	p_robot_node->links[robot_dir].length = length; // 2a. Uppdatera längd
	p_robot_node->links[robot_dir].p_node = p_node; // 2b. Koppla ny nod till gammal nod
		// 3.Uppdatera ny nod
	for(int i = 0; i < 4; i++)
	{
		p_node->links[i].open = ((open_walls >> (3 - i)) & 0x01); // 3a. Sätt rätt .open till true
	}
	p_node->links[(robot_dir + 2) % 4].p_node = p_robot_node; // 3b. Koppla gammal nod till ny nod
	p_node->links[(robot_dir + 2) % 4].length = length; // 3c. Längd för vägen bakåt satt
	p_robot_node = p_node; // 4.
		// 5. Nollställ avstånd (kanske sker utanför funktionen)
		// 6. Ta styrbslut och return (sker utanför funktionen)
}

// Create_origin
// Funktionen skapar en origonod som den returnerar en pekare till (Det är tänkt att robot_node sätts till detta returvärde när den skapas)
node* Create_origin(int open_walls)
{
	// Funktionen kan ändras beroende på hur start ser ut i labyrinten
	node* p_node = Newnode(0, 0); // Skapa nod
	p_node->start = true; // Sätt nod till startnod
	
	for(int i = 0; i < 4; i++) // Detta kanske ändras eller tas bort
	{
		p_node->links[i].open = ((open_walls >> (4 - i)) & 0x01); // Sätt rätt .open till true
	}
	return p_node;
}

// Map_main
// Innehållet kan ev. mergas med riktiga main när allt fungerar
int Map_main(void)
{
	PORTA = 0; // Port A används för att testa koden typ spårutskrifter alltså
	
	// Lite testning och pseudokod för hur allt kommer gå till
	// [undersöker riktningar (säg 1100)]
	node* p_robot_node = Create_origin(1100); // Skapa origo och robotpekare
	// [ta strybelsut (låt oss åka norr)]
	int robot_dir = 0; // Vi åker norr
	// [beordra styrbeslut]
	// [detekterar en helt ny nod]
	// [ev. beräkning av längd (låt den bli 7)]
	Create_node(0, 7, p_robot_node, robot_dir, 7, What_is_open(0, 1, 0, robot_dir)); // Skapa nod
	// (All information om kartan är sparad via robotpekaren och nås genom att följa robotpekaren)
	// [loopa]
	
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

// ev. funktion deleta allt allokerat minne mha free()
// (kanske inte behövs då vi inte ska deleta enskilda noder, och kan reseta minnet mellan körningar)
