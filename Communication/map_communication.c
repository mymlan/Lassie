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
//uint8_t sensor_front = 255; // i cm!
//uint8_t sensor_front_left = 1; // i mm!
//uint8_t sensor_front_right = 1; // i mm!
//uint8_t sensor_back_left = 1; // i mm!
//uint8_t sensor_back_right = 1; // i mm!
//uint8_t vinkelsensor = 1;
int test_variable_a;
int test_variable_b;

//int robot_dir = 0; // nord, öst, syd, väst
int NORTH = 0;
int EAST = 1;
int SOUTH = 2;
int WEST = 3;

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
	int cost;
	bool searched;
	//node* p_pre_dijk; // föregångare
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
	node *p_node = malloc(sizeof(node)); // Allokerar minne
	
	//for så många ööpningar, sen placera i array också, kanske if öppen på alla öppningar och directions, case i en case???
	
	//node *ps;
	//node *p_node = malloc(2 * sizeof(int) + sizeof(link) + number_of_roads * sizeof(node)); // Vad är det här?? // Detta allokerar en viss mängd minne för structen så att arrayen kan anta "godtycklig" storlek
	
	p_node->x = x_in;
	p_node->y = y_in;
	
	p_node->cost = 255; //representrar oändligheten
	p_node->searched = false; //inte avsökt från start
	//p_node->p_pre_dijk = NULL;
	
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

node* all_nodes[200]; // Sparar alla noder
node* p_robot_node;
int robot_dir;

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
		if(front > 700)
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
		if(front > 700)
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
		if(front > 700)
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
		if(front > 700)
		{
			buffer = buffer + 1;
		}
		return buffer;
	}
	return 0; //något har gått fel // man returnerar egentligen bara 0 när att gått bra. Andra siffror betyrer vissa felmeddelanden
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

// Create_goal
// Funktionen skapar en målnod som den returnerar en pekare till (Det är tänkt att robot_node sätts till detta returvärde när den skapas)
node* Create_goal(int x, int y, int open_walls)
{
	// Funktionen kan ändras beroende på hur start ser ut i labyrinten
	node* p_node = Newnode(x, y); // Skapa nod
	p_node->goal = true; // Sätt nod till startnod
		
	for(int i = 0; i < 4; i++) // Detta kanske ändras eller tas bort
	{
		p_node->links[i].open = ((open_walls >> (4 - i)) & 0x01); // Sätt rätt .open till true
	}
	return p_node;
}

// Create_node
// Skapar en ny nod kopplad till robotpekaren enligt givna argument. Uppdaterar p_robot_node så returvärde behövs inte.
node* Create_node(int x, int y, node* p_robot_node, int robot_dir, int length, int open_walls)
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
	return p_node; // 4.
		// 5. Nollställ avstånd (kanske sker utanför funktionen)
		// 6. Ta styrbslut och return (sker utanför funktionen)
}

// Update_node
// Uppdaterar given nod med värden som kopplar ihop noden med senaste nod
node* Update_node(node* p_node, node* p_robot_node, int robot_dir, int length)
{
	p_node->links[(robot_dir + 2) % 4].p_node = p_robot_node; // Ge noden pekar-info om förra noden
	p_node->links[(robot_dir + 2) % 4].length = length; // Ge noden längd-info om förra noden
	
	p_robot_node->links[robot_dir].p_node = p_node; // Ge förra noden pekar-info om noden
	p_robot_node->links[robot_dir].length = length; // Ge förra noden längd-info om noden
	
	return p_node;
}

// Existing_node_at
// Funktionen returnerar pekare till noden som finns på de givna koordinaterna om det finnsen nod där. Annars returneras NULL;
node* Exisiting_node_at(int x, int y)
{
	int i = 0;
	while (i < 200 && all_nodes[i] != NULL)
	{
		if (all_nodes[i]->x == x && all_nodes[i]->y == y)
		{
			return	all_nodes[i];
		}
	}
	return NULL;
}

// Get_decision_searching
//början av funktion för att genera styrbeslut
//kanske får brytas ner till fler funktioner istället då den borde bli stor
//se designspec för numerisk representation av styrbeslut, ex 1->åk fram.
//Men den kanske inte ens ser ut såhär, den får stå kvar sålänge
int Get_decision_searching(uint8_t senleftfront, uint8_t senleftback, uint8_t senfront, uint8_t senrightfront, uint8_t senrightback)
{	
	if(senleftfront < 200 && senrightfront < 200 && senfront < 20) // Kollar 2 cm innan vägg om återvändsgränd
	{
		Create_node(5, 6, NULL, NORTH, 42, What_is_open(201, 201, 201, NORTH));
		// Ge order om att backa
	}
	if (senleftfront + senleftback + senrightfront + senrightback < 600 && senfront < 20)
	{
		return 0;
	}
	// Annars ingen intressant punk och vi tar inget nytt beslut.
	return 0;
}

//Dijkstras algoritm, första steg att räkna avståndet, borde utvecklas til att hitta vägen/körrikting också.
//bara skräpkod i just nu för att det ska kompilera
int Dijk(node* p_node1, node* p_node2)
{
//1. Alla noder är markerade ej avsökra direkt i struct
	if(p_node1->cost == p_node2->cost)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

// Sensor_values_has_arrived
// Denna kod körs varje gång sensorvärden kommer. Koden kan senare ev. flyttas till mainloopen när allt fungerar.
// När sensorvärden kommer, kör denna kod. Koden avgör om man är i en korsning och beroende på om det är en ny eller gammal korsning skapas eller uppdateras noden.
// Det finns avsatta rader där strybeslut skickas till styrmodulen.
void Sensor_values_has_arrived(uint8_t sensor_front, uint8_t sensor_front_left, uint8_t sensor_front_right, uint8_t sensor_back_left, uint8_t sensor_back_right, int length)
{
	// Uträkning av koordinater
	int x = p_robot_node->x;
	int y = p_robot_node->y;
	int traveled_blocks = (length + 20) / 40; // Antalet färdade rutor
	switch (robot_dir)
	{
		case 0:
		{
			int y = y + traveled_blocks;
			break;
		}
		case 1:
		{
			int y = y - traveled_blocks;
			break;
		}
		case 2:
		{
			int x = x + traveled_blocks;
			break;
		}
		case 3:
		{
			int x = x - traveled_blocks;
			break;
		}
	}
	
	// -------- Detta är kartlagring och genomsökning ---------- //
	// HÄNDELSE: Sensorvärde inkommit
	// Indikerar sensorvärden återvändsgränd?
	if (sensor_front < 10 && sensor_front_left < 200 && sensor_front_right < 200) // 10 cm så vi kommer nära väggen och klarar detektera en ev. RFID
	{
		// Skapa nod
		Create_node(x, y, p_robot_node, robot_dir, length, What_is_open(sensor_front_left, sensor_front_right, sensor_front, robot_dir));
		// "HOPP": Åk utforskad väg till närmsta outforskade nod
	}
	// Annars, indikerar bakre sensorer en korsning?
	else if (sensor_back_left > 20 || sensor_back_right > 20)
	{
		// Är det en ny korsning?
		if (Exisiting_node_at(x, y)) // Ska bli generell (true)
		{
			// Skapa nod utifrån främre 3 sensorer
			Create_node(x, y, p_robot_node, robot_dir, length, What_is_open(sensor_front_left, sensor_front_right, sensor_front, robot_dir));
			// Öppet rakt fram?
				// Ingen förändring av styrbeslut
			// Annars, öppet höger?
			if (sensor_front_right > 200)
			{
				// Ge order om sväng höger
				// Vänta
				// HÄNDELSE: 90 grader klart
				// Kör rakt fram
			}
			// Annars (öppet vänster)
			else
			{
				// Ge order om svänga vänster
				// Vänta
				// HÄNDELSE: 90 grader klart
				// Kör rakt fram
			}
		}
		// Annars (gammal korsning)
		else
		{
			// Uppdatera nod
			Update_node(Exisiting_node_at(x, y), p_robot_node, robot_dir, length);
			// Möjligt att åka rakt fram?
			if (sensor_front > 30)
			{
				// Ge order om att åka rakt fram
			}
			// Annars (vi står inte i en korsning med en outforskad väg)
			else
			{
				// "HOPP": Åk utforskad väg till närmsta outforskade nod
			}
		}
	}
}

// Map_main
// Innehållet kan ev. mergas med riktiga main när allt fungerar
int Map_main(void)
{
	// Lite testning och pseudokod för hur allt kommer gå till
	// FÖRKLARING: i what is open är 255 (avstånd) "öppet", och 0 (avstånd) "vägg". NORTH, WEST etc definierade som 0, 3 etc.
	// [undersöker riktningar (säg 1100)]
	p_robot_node = Create_origin(1100); // Skapa origo och robotpekare
	// [ta strybelsut (låt oss åka norr)]
	robot_dir = 0; // Vi åker norr
	// [beordra styrbeslut]
	// [detekterar en helt ny nod]
	// [ev. beräkning av längd (låt den bli 7)]
	p_robot_node = Create_node(0, 7, p_robot_node, robot_dir, 7, What_is_open(0, 255, 0, robot_dir)); // Skapa nod
	// (All information om kartan är sparad via robotpekaren och nås genom att följa robotpekaren)
	// [loopa]
	test_variable_a = p_robot_node->x; // Ska va 0
	test_variable_b = p_robot_node->y; // Ska va 7
	
	robot_dir = 1; // Vi åker öster
	p_robot_node = Create_node(3, 7, p_robot_node, robot_dir, 3, What_is_open(255, 255, 0, robot_dir)); // Vi har åkt höger och skapat en nod.
	test_variable_a = p_robot_node->x; // Ska va 3
	test_variable_b = p_robot_node->y; // Ska va 7
	
	robot_dir = 0; // Vi åker norr
	p_robot_node = Create_node(3, 9, p_robot_node, robot_dir, 3, What_is_open(255, 255, 0, robot_dir)); // Vi har åkt vänster och skapat en nod.
	test_variable_a = p_robot_node->links[SOUTH].p_node->links[WEST].p_node->x; // Ska va 0 (förrförra noden)
	test_variable_b = p_robot_node->links[SOUTH].p_node->links[WEST].p_node->y; // Ska va 7 (förrförra noden)
	
	//test_variable_a = p_robot_node->links[2].open; // Ska va 1 // fungerar
	//test_variable_b = p_robot_node->links[3].open; // Ska va 1 // fungerar
	
	robot_dir = 1;// Vi åker öster
	p_robot_node = Create_node(5, 9, p_robot_node, robot_dir, 3, What_is_open(0, 0, 0, robot_dir)); // Vi har åkt höger och skapat en nod.
	
	/* // Behövs ej om all_nodes redan är fyllt med NULL istället för skräp
	for (int i = 0; i < 200; i = i + 1)
	{
		all_nodes[i] = NULL;
	}
	*/
	
	// Dessa olika "modes" finns:
		// Ren genomsökning och skapande av nya noder
		// Åkning från en nod till en annan. (Ex1: Åkning till outforskad nod, ex2: åkning till start, ex3: åkning till mål
		// Beräkning av kortaste väg
	

		// --------- Åkning en redan utforskad väg ------ //
		// Detta är en annan historia...
	
	
	
	// ------------- Test hur många noder som får plats i minnet ------------- //
	test_variable_a = 0;
	while (malloc(sizeof(node)) != NULL)
	{
		test_variable_a = test_variable_a + 1;
	}
	
	test_variable_b = p_robot_node->links[0].open; // Ska va 0
	test_variable_b = p_robot_node->links[1].open; // Ska va 1
	
	
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
