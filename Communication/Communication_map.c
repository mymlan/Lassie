#define F_CPU 18432000UL

#include <avr/io.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include "../CommonLibrary/Common.h"
#include "Communication_map.h"
#include "Communication_SPI.h"
#include "Firefly.h"
#include "Communication_init.h"
#include <util/delay.h>

//uint8_t start_regulating = TRUE;

// Newnode
// Allokerar minne för noden och sätter vissa egenskaper för noden.
node* Newnode(uint8_t x_coordinate_in, uint8_t y_coordinate_in)
{	
	node *p_node = malloc(sizeof(node)); // Allokerar minne
	
	//for så många ööpningar, sen placera i array också, kanske if öppen på alla öppningar och directions, case i en case???
	
	//node *ps;
	//node *p_node = malloc(2 * sizeof(int) + sizeof(link) + number_of_roads * sizeof(node)); // Vad är det här?? // Detta allokerar en viss mängd minne för structen så att arrayen kan anta "godtycklig" storlek
	
	p_node->x_coordinate = x_coordinate_in;
	p_node->y_coordinate = y_coordinate_in;
	
	p_node->cost = 255; //representrar oändligheten
	p_node->searched = FALSE; //inte avsökt från start
	//p_node->p_pre_dijk = NULL;
	
	p_node->start = FALSE;
	p_node->goal = FALSE;
	
	p_node->links[0].open = FALSE;
	p_node->links[1].open = FALSE;
	p_node->links[2].open = FALSE;
	p_node->links[3].open = FALSE;
	
	p_node->links[0].length = 0;
	p_node->links[1].length = 0;
	p_node->links[2].length = 0;
	p_node->links[3].length = 0;
	
	p_node->links[0].p_node = NULL;
	p_node->links[1].p_node = NULL;
	p_node->links[2].p_node = NULL;
	p_node->links[3].p_node = NULL;
	
	all_nodes[all_nodes_size] = p_node;
	all_nodes_size++;
	
	return p_node;
}

//vill skriva funktion som tar in riktning och sensorvärden för att bestämma vilka riktningar/väderstreck som är öppna
//returnerar ett tal på 4 bitar i ordningen nord,öst,syd,väst -> 1111, rdir är robtens riktning 0-3 i ordningen nord,öst,syd,väst
//jag vet inte om det finns snyggare samband, men jag går igenom alla fall till att börja med
uint8_t What_is_open(uint8_t left, uint8_t right, uint8_t front)
{
	if(robot_dir == NORTH) // om vi åker norrut
	{
		uint8_t buffer = 2; // alltid öppet bakåt
		if(front > SIZE_OF_SQUARE_IN_CM)
		{
			buffer = buffer + 8;
		}
		if(right > SIDE_SENSOR_OPEN_LIMIT)
		{
			buffer = buffer + 4;
		}
		if(left > SIDE_SENSOR_OPEN_LIMIT)
		{
			buffer = buffer + 1;
		}
		test_variable_b = buffer;
		return buffer;
	}
	else if(robot_dir == EAST)//om vi åker österut
	{
		uint8_t buffer = 1; // alltid öppet bakåt
		if(left > SIDE_SENSOR_OPEN_LIMIT)
		{
			buffer = buffer + 8;
		}
		if(front > SIZE_OF_SQUARE_IN_CM)
		{
			buffer = buffer + 4;
		}
		if(right > SIDE_SENSOR_OPEN_LIMIT)
		{
			buffer = buffer + 2;
		}
		return buffer;
	}
	else if(robot_dir == SOUTH)//om vi åker söderut
	{
		uint8_t buffer = 8; // alltid öppet bakåt
		if(left > SIDE_SENSOR_OPEN_LIMIT)
		{
			buffer = buffer + 4;
		}
		if(front > SIZE_OF_SQUARE_IN_CM)
		{
			buffer = buffer + 2;
		}
		if(right > SIDE_SENSOR_OPEN_LIMIT)
		{
			buffer = buffer + 1;
		}
		return buffer;
	}
	
	else if(robot_dir == WEST)//om vi åker västerut
	{
		uint8_t buffer = 4; // alltid öppet bakåt
		if(right > SIDE_SENSOR_OPEN_LIMIT)
		{
			buffer = buffer + 8;
		}
		if(left > SIDE_SENSOR_OPEN_LIMIT)
		{
			buffer = buffer + 2;
		}
		if(front > SIZE_OF_SQUARE_IN_CM)
		{
			buffer = buffer + 1;
		}
		return buffer;
	}
	SPI_map_send_command_to_steering(ID_BYTE_AUTO_DECISIONS, COMMAND_STOP_1);
	_delay_ms(5000);
	return ERROR_IN_WHAT_IS_OPEN; //något har gått fel // man returnerar egentligen bara 0 när att gått bra. Andra siffror betyrer vissa felmeddelanden
}

// Create_origin
// Funktionen skapar en origonod som den returnerar en pekare till (Det är tänkt att robot_node sätts till detta returvärde när den skapas)
void Create_origin(uint8_t open_walls)
{
	// Funktionen kan ändras beroende på hur start ser ut i labyrinten
	Create_node(30, 30, 0, open_walls); // Skapa nod
	p_robot_node->start = TRUE; // Sätt nod till startnod
	p_start_node = p_robot_node;
	/*
	node* p_node = Newnode(30, 0); // Skapa nod
	
	p_node->start = TRUE; // Sätt nod till startnod
	
	for(int i = 0; i < NUMBER_OF_LINKS; i++) // Detta kanske ändras eller tas bort
	{
		p_node->links[i].open = ((open_walls >> (3 - i)) & 0x01); // Sätt rätt .open till TRUE
	}
	p_robot_node = p_node;
	*/
}

// Create_node
// Skapar en ny nod kopplad till robotpekaren enligt givna argument. Uppdaterar p_robot_node så returvärde behövs inte.
void Create_node(uint8_t x_coordinate, uint8_t y_coordinate, uint8_t length, uint8_t open_walls)
{
	//SPI_map_send_command_to_steering(ID_BYTE_AUTO_DECISIONS, COMMAND_FORWARD_NOT_REGULATED);
	//start_regulating = TRUE;
	// 1.Skapa ny nod
	node* p_node = Newnode(x_coordinate, y_coordinate); // 1.
	// 2.Uppdatera gammal nod
	p_robot_node->links[robot_dir].length = length; // 2a. Uppdatera längd
	p_robot_node->links[robot_dir].p_node = p_node; // 2b. Koppla ny nod till gammal nod
	// 3.Uppdatera ny nod
	for(int i = 0; i < NUMBER_OF_LINKS; i++)
	{
		p_node->links[i].open = ((open_walls >> (3 - i)) & 0x01); // 3a. Sätt rätt .open till TRUE
	}
	p_node->links[(robot_dir + 2) % NUMBER_OF_LINKS].p_node = p_robot_node; // 3b. Koppla gammal nod till ny nod
	p_node->links[(robot_dir + 2) % NUMBER_OF_LINKS].length = length; // 3c. Längd för vägen bakåt satt
	if (p_node->x_coordinate == 30 && p_node->y_coordinate == 30)
	{
		Map_send_link_coordinates_to_PC(ID_BYTE_LINK_COORDINATES, 30, 30, x_coordinate, y_coordinate);
	}
	else
	{
		Map_send_link_coordinates_to_PC(ID_BYTE_LINK_COORDINATES, p_robot_node->x_coordinate, p_robot_node->y_coordinate, x_coordinate, y_coordinate);
	}
	
	p_robot_node = p_node;// 4.
	// 5. Nollställ avstånd (kanske sker utanför funktionen)
	// 6. Ta styrbslut och return (sker utanför funktionen)
	enable_node_editing = FALSE;
	/*
	// TESTKOD
	SPI_map_send_command_to_steering(ID_BYTE_AUTO_DECISIONS, COMMAND_STOP);
	_delay_ms(250);
	SPI_map_send_command_to_steering(ID_BYTE_AUTO_DECISIONS, COMMAND_FORWARD);
	*/
}

// Create_goal
// Funktionen skapar en målnod som den returnerar en pekare till (Det är tänkt att robot_node sätts till detta returvärde när den skapas)
void Create_goal(uint8_t x_coordinate, uint8_t y_coordinate,uint8_t length, uint8_t open_walls)
{
	// Funktionen kan ändras beroende på hur start ser ut i labyrinten
	Create_node(x_coordinate, y_coordinate, length, open_walls); // Skapa nod
	p_robot_node->goal = TRUE; // Sätt nod till startnod
	p_goal_node = p_robot_node;
}

// Update_node
// Uppdaterar given nod med värden som kopplar ihop noden med senaste nod
void Update_node(node* p_node, uint8_t length)
{
	//SPI_map_send_command_to_steering(ID_BYTE_AUTO_DECISIONS, COMMAND_FORWARD_NOT_REGULATED);
	//start_regulating = TRUE;
	
	p_node->links[(robot_dir + 2) % NUMBER_OF_LINKS].p_node = p_robot_node; // Ge noden pekar-info om förra noden
	p_node->links[(robot_dir + 2) % NUMBER_OF_LINKS].length = length; // Ge noden längd-info om förra noden
	
	p_robot_node->links[robot_dir].p_node = p_node; // Ge förra noden pekar-info om noden
	p_robot_node->links[robot_dir].length = length; // Ge förra noden längd-info om noden
	
	Map_send_link_coordinates_to_PC(ID_BYTE_LINK_COORDINATES, p_robot_node->x_coordinate, p_robot_node->y_coordinate, p_node->x_coordinate, p_node->y_coordinate);
	
	p_robot_node = p_node;
	
	enable_node_editing = FALSE;
	
	/*
	// TESTKOD
	SPI_map_send_command_to_steering(ID_BYTE_AUTO_DECISIONS, COMMAND_STOP);
	_delay_ms(250);
	SPI_map_send_command_to_steering(ID_BYTE_AUTO_DECISIONS, COMMAND_FORWARD);
	*/
}

// Existing_node_at
// Funktionen returnerar pekare till noden som finns på de givna koordinaterna om det finnsen nod där. Annars returneras NULL;
node* Exisiting_node_at(uint8_t x_coordinate, uint8_t y_coordinate)
{
	for (int i = 0; i < all_nodes_size; i++)
	{
		if (all_nodes[i]->x_coordinate == x_coordinate && all_nodes[i]->y_coordinate == y_coordinate)
		{
			return all_nodes[i];
		}
	}
	return NULL;
}

// Hittar placeringen som noden ligger på i all_nodes // Kan tas bort senare om den inte används
uint8_t Find_index_of_node(node* p_node)
{
	for (int i = 0; i < all_nodes_size ; i++)
	{
		if(all_nodes[i] == p_node)
		{
			return i;
		}
	}
	return ERROR_IN_FIND_INDEX_OF_NODE;
}

//Hjälpfunktion till Dijkstras, hittar den nod som har lägst kostnad och som inte är avsökt.
uint8_t Find_low_cost_index()
{
	uint8_t temp_cost = DIJKSTRA_INFINITY;
	uint8_t temp_index = DIJKSTRA_INFINITY;
	for(int i = 0; i < all_nodes_size; i++)
	{
		if(all_nodes[i]->searched == FALSE && all_nodes[i]->cost < temp_cost)
		{
			temp_cost = all_nodes[i]->cost;
			temp_index = i;
		}
	}
	return temp_index;
}

//Dijkstras algoritm, första steg att räkna avståndet, borde utvecklas til att hitta vägen/körrikting också.
//skräpkod i lsutet för att det ska kompilera
//Behöver kanske inte returnera någonting, pekare ochkostnad finns i noden? Avbryta nör nod2 avsökt?
uint8_t Find_shortest_path(node* p_node1, node* p_node2)
{
	
	//1. Markerar alla noder ej avsökta och sätter kostnaden till oändlighten samt sätter föregångaren som odefinerad
	//Sätter även startnodens kostnad till 0.
	for(int i = 0; i < all_nodes_size; i++)
	{
		all_nodes[i]->searched = FALSE;
		all_nodes[i]->cost = DIJKSTRA_INFINITY;// =102 m =~ DIJKSTRA_INFINITY
		all_nodes[i]->p_pre_dijk = NULL;
	}
	if (p_node1 == p_node2)
	{
		return 0;
	}
	p_node1->cost = 0;
	
	if (p_node2 == NULL)
	{
		return 0;
	}
	
	node* p_chosen_node;
	
	while(p_node2->searched == FALSE)
	{
		//2. Hitta den nod som har lägst nodpris, första gången startnoden.
		p_chosen_node = all_nodes[Find_low_cost_index()];
		//3. Ge angränsande noder uppdaterade värden om deras kostnad och föregångare.
		for(int n = 0; n < NUMBER_OF_LINKS; n++)
		{
			if(p_chosen_node->links[n].p_node != NULL && p_chosen_node->links[n].p_node->cost > p_chosen_node->cost + p_chosen_node->links[n].length)
			{
				p_chosen_node->links[n].p_node->cost = p_chosen_node->cost + p_chosen_node->links[n].length;
				p_chosen_node->links[n].p_node->p_pre_dijk = p_chosen_node;
			}
		}
		//4. Sätt vald nod som genomsökt
		p_chosen_node->searched = TRUE;
	}

	return p_node2->cost;
}

// Get_cardinal_direction
// Funktionen omvandlar robotens riktning och dess sväng (rakt, vänster, höger, back) till ett vädersträck
uint8_t Get_cardinal_direction(uint8_t robot_dir, uint8_t turn) // no turn = 0, turn right = 1, back = 2, turn left = 3
{
	return (robot_dir + turn) % NUMBER_OF_LINKS;
}

// Wait_for_90_degree_rotation
// Funktionen loopar till sensor meddelar 90 grader klart
void Wait_for_90_degree_rotation()
{
	cli();
	uint8_t go = SPI_map_should_handle_rotation_finished();
	sei();
	while(go == FALSE)
	{
		cli();
		go = SPI_map_should_handle_rotation_finished();
		sei();
	}
	//SPI_map_send_command_to_steering(ID_BYTE_AUTO_DECISIONS, COMMAND_STOP);
}

// Do_turn
// Funktonen utför en sväng eller liknande för att rotera roboten i given riktning genom anrop till styr och sensormodulerna
void Do_turn(uint8_t cardinal_direction)
{
	switch ((robot_dir - cardinal_direction + NUMBER_OF_LINKS) % NUMBER_OF_LINKS)
	{
		case 3:
		{
			SPI_map_send_command_to_steering(ID_BYTE_AUTO_DECISIONS, COMMAND_STOP);
			_delay_ms(100);
			
			// Turn right order
			SPI_map_send_id_byte_to_sensor(ID_BYTE_START_ANGULAR_RATE_SENSOR);
			SPI_map_send_command_to_steering(ID_BYTE_AUTO_DECISIONS, COMMAND_TIGHT_TURN_RIGHT);
			Map_send_byte_to_PC(ID_BYTE_AUTO_DECISIONS, COMMAND_TIGHT_TURN_RIGHT);
			Wait_for_90_degree_rotation();
			
			robot_dir = (robot_dir + 1) % NUMBER_OF_LINKS;
			length = 0;
			break;
		}
		case 2:
		{
			SPI_map_send_command_to_steering(ID_BYTE_AUTO_DECISIONS, COMMAND_STOP);
			_delay_ms(150);
			
			// Rotation 180 degrees order
			SPI_map_send_id_byte_to_sensor(ID_BYTE_START_ANGULAR_RATE_SENSOR);
			SPI_map_send_command_to_steering(ID_BYTE_AUTO_DECISIONS, COMMAND_ROTATE_RIGHT);
			Map_send_byte_to_PC(ID_BYTE_AUTO_DECISIONS, COMMAND_ROTATE_RIGHT);
			Wait_for_90_degree_rotation(); // 90 grader
			SPI_map_send_id_byte_to_sensor(ID_BYTE_START_ANGULAR_RATE_SENSOR);
			SPI_map_send_command_to_steering(ID_BYTE_AUTO_DECISIONS, COMMAND_ROTATE_RIGHT);
			Wait_for_90_degree_rotation(); // ytterligare 90 grader
			_delay_ms(75);
			
			SPI_map_send_command_to_steering(ID_BYTE_AUTO_DECISIONS, COMMAND_STOP);
			_delay_ms(70);
			
			robot_dir = (robot_dir + 2) % NUMBER_OF_LINKS;
			length = 0;
			break;
		}
		case 1:
		{
			SPI_map_send_command_to_steering(ID_BYTE_AUTO_DECISIONS, COMMAND_STOP);
			_delay_ms(100);
			// Turn left order
			SPI_map_send_id_byte_to_sensor(ID_BYTE_START_ANGULAR_RATE_SENSOR);
			SPI_map_send_command_to_steering(ID_BYTE_AUTO_DECISIONS,COMMAND_TIGHT_TURN_LEFT);
			Map_send_byte_to_PC(ID_BYTE_AUTO_DECISIONS, COMMAND_TIGHT_TURN_LEFT);
			Wait_for_90_degree_rotation();
			
			robot_dir = (robot_dir + 3) % NUMBER_OF_LINKS;
			length = 0;
			/*
			SPI_map_send_command_to_steering(ID_BYTE_AUTO_DECISIONS, COMMAND_FORWARD);
			_delay_ms(400);
			SPI_map_send_command_to_steering(ID_BYTE_AUTO_DECISIONS, COMMAND_STOP);
			_delay_ms(1);
			*/
			break;
		}
		case 0:
		{
			length = 0;
			break;
		}
		default:
		SPI_map_send_command_to_steering(ID_BYTE_AUTO_DECISIONS, COMMAND_STOP_1);
		_delay_ms(2000);
		break;
	}
	// Åk fram oreglerat order
	SPI_map_send_command_to_steering(ID_BYTE_AUTO_DECISIONS, COMMAND_FORWARD); // Not regulated kanske
	Map_send_byte_to_PC(ID_BYTE_AUTO_DECISIONS, COMMAND_FORWARD);
}

// Get_dijkpointers_cardinal_direction
// Funktionen tar in en nodpekare och returnerar dess dijkstraspekares vädersträck
uint8_t Get_dijkpointers_cardinal_direction(node* p_node)
{
	for (int i = 0; i < NUMBER_OF_LINKS; i++)
	{
		if (p_node->links[i].p_node->x_coordinate == p_node->p_pre_dijk->x_coordinate && p_node->links[i].p_node->y_coordinate == p_node->p_pre_dijk->y_coordinate)
		{
			return i;
		}
	}
	return ERROR_IN_GET_DIJKPOINTERS_CARDINAL_DIRECTION;
}

// Follow_path
// Funktionen utför en sväng enligt dijkstraspekarna när roboten når en korsning
void Follow_path() //uint8_t sensor_back_left, uint8_t sensor_back_right§
{
	//indikerar bakre sensorer en korsning

	
	if (p_robot_node->p_pre_dijk == NULL)
	{
		following_path = FALSE;
		for (int i = 0; i < NUMBER_OF_LINKS; i++)
		{
			if (p_robot_node->links[i].length == 0 && p_robot_node->links[i].open)
			{
				Do_turn(i);
				break;
			}
		}
	}
	else
	{
		Do_turn(Get_dijkpointers_cardinal_direction(p_robot_node));
	}
}

// Hittar en nod som har en outforskad öppning
node* Easy_find_unexplored_node()
{
	for (int i = all_nodes_size - 1; i > 0; i--)
	{
		for (int n = 0; n < NUMBER_OF_LINKS; n++)
		{
			if (all_nodes[i]->links[n].open && all_nodes[i]->links[n].length == 0) // Om öppen med längd 0, alltså outforskad.
			{
				return all_nodes[i];
			}
		}
	}
	if(level == KEEP_SEARCHING)
	{
		Find_shortest_path(p_start_node, p_robot_node);
		following_path = TRUE;	
		level = RETURN_AFTER_FOUND; // Level up 2->3	
	}
	if(level == SEARCH_FOR_GOAL)
	{
		Find_shortest_path(p_start_node, p_robot_node);
		following_path = TRUE;
		level = 42; // Om Lassie inte hittat RFID
		SPI_map_send_command_to_steering(ID_BYTE_AUTO_DECISIONS, COMMAND_FORWARD);
	}
	return all_nodes[0]; // åker till start om upptäckt hela, levlar innan åker hem
}

//En smartare variant av Easy_find_unexplored_node
//Hittar istället den nod som är minst antal steg från given nod(oftast robotpekaren som anropas)
node* Smarter_find_unexplored_node(node* p_node)
{
	for(int i = 0; i < NUMBER_OF_LINKS; i++) //kolar om någon link är outforskad
	{
		if(p_node->links[i].open && p_node->links[i].length == 0) // öppen och längd 0 -> outforskad väg
		{
			return p_node;
		}
	}
	for(int n = 0; n < NUMBER_OF_LINKS; n++) //Om inte hittad outforskad, gör samma sak på grannarna
	{
		if(p_node->links[n].p_node != NULL && !p_node->links[n].p_node->start) //Om nästa nod inte är NULL och inte startnod
		{
			return Smarter_find_unexplored_node(p_node->links[n].p_node); //Så ska genomsökning på den noden ske.
		}
	}
	return all_nodes[0]; // åker till start om upptäckt hela
}

// Get_length
// Funktionen loopar tills length kommer, och returnerar då length
uint8_t Get_length()
{
	SPI_map_send_id_byte_to_sensor(ID_BYTE_GIVE_DISTANCE);
	while (SPI_map_should_handle_new_distance() == FALSE) // Sätts till TRUE av KOM i ett avbrott
	{
	}
	return communication_distance;
}

//initieringsfunktion, ska köras i början av huvudloopen i Communication.c
void Map_init()
{
	all_nodes_size = 0;
	following_path = 0;
	level = 0;
	enable_node_editing = 1;
	robot_dir = NORTH;
	enable_safety = 0;
}

// Map_main
// Innehållet kan ev. mergas med riktiga main när allt fungerar
int Map_main()
{

	Create_origin(What_is_open(100, 100, 70)); // 0,0
	a = p_robot_node->x_coordinate;
	b = p_robot_node->y_coordinate;
	Update_map(70, 100, 100, 100, 100); // 0,1
	Update_map(70, 100, 100, 100, 100); // 0,1
	Update_map(70, 100, 100, 100, 100); // 0,1
	Update_map(70, 100, 100, 100, 100); // 0,1
	a = p_robot_node->x_coordinate;
	b = p_robot_node->y_coordinate;
	Update_map(30, 255, 255, 100, 100); // 0,2
	Update_map(10, 255, 255, 255, 255); // 0,2

	a = p_robot_node->x_coordinate;
	b = p_robot_node->y_coordinate;
	Update_map(70, 100, 100, 100, 100); // 0,3
	Update_map(70, 100, 100, 100, 100);
	a = p_robot_node->x_coordinate;
	b = p_robot_node->y_coordinate;
	Update_map(70, 100, 255, 100, 255); // 0,4
	a = p_robot_node->x_coordinate;
	b = p_robot_node->y_coordinate;

	Update_map(70, 100, 100, 100, 100); // 0,5
	a = p_robot_node->x_coordinate;
	b = p_robot_node->y_coordinate;
	Update_map(5, 100, 100, 100, 100); // 0,6
	a = p_robot_node->x_coordinate;
	b = p_robot_node->y_coordinate;
	Update_map(70, 100, 100, 100, 100); // 0,5
	Update_map(70, 100, 100, 100, 100); // 0,5
	Update_map(70, 100, 100, 100, 100); // 0,5
	Update_map(70, 100, 100, 100, 100); // 0,5
	a = p_robot_node->x_coordinate;
	b = p_robot_node->y_coordinate;
	Update_map(70, 255, 100, 255, 100); // 0,4
	Update_map(70, 100, 100, 100, 100); // 
	a = p_robot_node->x_coordinate;
	b = p_robot_node->y_coordinate;
	Update_map(70, 100, 255, 100, 255); // 2,4
	Update_map(70, 100, 100, 100, 100); // 
	a = p_robot_node->x_coordinate;
	b = p_robot_node->y_coordinate;
	Update_map(20, 255, 255, 100, 255); // 4,4 %%%%%
	Update_map(70, 100, 100, 100, 100); // 
	a = p_robot_node->x_coordinate;
	b = p_robot_node->y_coordinate;
	Update_map(20, 100, 255, 100, 255); // 4,2
	Update_map(70, 100, 100, 100, 100); // 
	a = p_robot_node->x_coordinate;
	b = p_robot_node->y_coordinate;
	Update_map(70, 100, 255, 100, 255); // 2,2
	Update_map(70, 100, 100, 100, 100); // 
	a = p_robot_node->x_coordinate;
	b = p_robot_node->y_coordinate;
	Update_map(70, 255, 255, 255, 255); // 0,2 %%%%
	Update_map(70, 100, 100, 100, 100); // 
	a = p_robot_node->x_coordinate;
	b = p_robot_node->y_coordinate;
	Update_map(70, 255, 100, 255, 100); // 2,2
	Update_map(70, 100, 100, 100, 100); // 
	a = p_robot_node->x_coordinate;
	b = p_robot_node->y_coordinate;
	Update_map(20, 255, 255, 255, 255); //2,4 
	a = p_robot_node->x_coordinate;
	b = p_robot_node->y_coordinate;
	Update_map(20, 255, 255, 255, 255); // 4,4
	a = p_robot_node->x_coordinate;
	b = p_robot_node->y_coordinate;
	Update_map(5, 100, 100, 100, 100); // 4,6
	
	a = p_robot_node->x_coordinate;
	b = p_robot_node->y_coordinate;
	Update_map(70, 100, 255, 100, 255); // 4,4
	a = p_robot_node->x_coordinate;
	b = p_robot_node->y_coordinate;
	Update_map(70, 255, 100, 255, 100); // 2,4
	a = p_robot_node->x_coordinate;
	b = p_robot_node->y_coordinate;
  	Update_map(20, 255, 255, 255, 255); // 0,4
	a = p_robot_node->x_coordinate;
	b = p_robot_node->y_coordinate;
	Update_map(70, 255, 255, 255, 255); // 0,2
	a = p_robot_node->x_coordinate;
	b = p_robot_node->y_coordinate;
	Update_map(20, 100, 255, 100, 255); // -2,2
	a = p_robot_node->x_coordinate;
	b = p_robot_node->y_coordinate;
	Update_map(20, 255, 100, 255, 100); // -2,4
	a = p_robot_node->x_coordinate;
	b = p_robot_node->y_coordinate;
	Update_map(5, 100, 100, 100, 100); // -4,4
	a = p_robot_node->x_coordinate;
	b = p_robot_node->y_coordinate;
	Update_map(20, 100, 255, 100, 255); // -2,4
	
	a = p_robot_node->x_coordinate;
	b = p_robot_node->y_coordinate;
	Update_map(20, 255, 100, 255, 100); // -2,2
	a = p_robot_node->x_coordinate;
	b = p_robot_node->y_coordinate;
	Update_map(70, 255, 255, 255, 255); //0,2 
	a = p_robot_node->x_coordinate;
	b = p_robot_node->y_coordinate;
	Update_map(70, 100, 100, 100, 100); // 0,0
	a = p_robot_node->x_coordinate;
	b = p_robot_node->y_coordinate;
	
	
	
	
	
	Update_map(70, 255, 100, 255, 100); //0,2 
	a = p_robot_node->x_coordinate;
	b = p_robot_node->y_coordinate;
	Update_map(70, 100, 100, 100, 100); // 1,2
	a = p_robot_node->x_coordinate;
	b = p_robot_node->y_coordinate;
	Update_map(70, 255, 100, 255, 100); //2,2 
	a = p_robot_node->x_coordinate;
	b = p_robot_node->y_coordinate;
	Update_map(70, 100, 100, 100, 100); // 2,3
	a = p_robot_node->x_coordinate;
	b = p_robot_node->y_coordinate;
	Update_map(70, 255, 255, 255, 255); //2,4 
	a = p_robot_node->x_coordinate;
	b = p_robot_node->y_coordinate;
	Update_map(70, 100, 100, 100, 100); // 2,5
	a = p_robot_node->x_coordinate;
	b = p_robot_node->y_coordinate;
	Update_map(5, 100, 100, 100, 100); //2,6 
	a = p_robot_node->x_coordinate;
	b = p_robot_node->y_coordinate;
	
	
	a = Find_shortest_path(all_nodes[2], all_nodes[0]);
	b = all_nodes[0]->p_pre_dijk->x_coordinate;
	c = all_nodes[0]->p_pre_dijk->y_coordinate;
	d = all_nodes[0]->p_pre_dijk->p_pre_dijk->x_coordinate;
	e = all_nodes[0]->p_pre_dijk->p_pre_dijk->y_coordinate;
	
	
	
	test_variable_a = 0;
	//Sensor_values_has_arrived();
	
	
	/*
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
	*/
	
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

//---------------Påbörjar ny kod här-------------------//
//returnerar true om sensorerna känner av korsning, annars falskt.
uint8_t Crossing(uint8_t sensor_front, uint8_t sensor_front_left, uint8_t sensor_front_right, uint8_t sensor_back_left, uint8_t sensor_back_right)
{
	//Korsning åt vänster
	if(sensor_front_left > SIDE_SENSOR_OPEN_LIMIT && sensor_back_left > SIDE_SENSOR_OPEN_LIMIT)
	{
		return TRUE;
	}
	//korsning åt höger
	else if(sensor_front_right > SIDE_SENSOR_OPEN_LIMIT && sensor_back_right > SIDE_SENSOR_OPEN_LIMIT)
	{
		return TRUE;
	}
	//återvändsgränd
	else if(sensor_front < FRONT_SENSOR_LIMIT && sensor_front_left < SIDE_SENSOR_OPEN_LIMIT && sensor_front_right < SIDE_SENSOR_OPEN_LIMIT && sensor_back_left < SIDE_SENSOR_OPEN_LIMIT && sensor_back_right < SIDE_SENSOR_OPEN_LIMIT)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}	
}

// Funktion som körs om following path och korsning gäller
void Following_path_at_crossing()
{	
	uint8_t turn_done = FALSE;
	if(p_robot_node->p_pre_dijk == NULL) // Är korsningen vårt mål?
	{
		if (level == 42)
		{
			SPI_map_send_command_to_steering(ID_BYTE_AUTO_DECISIONS, COMMAND_STOP);

			level = 43;
		}
		else if (level == RETURN_AFTER_FOUND)
		{

			SPI_map_send_command_to_steering(ID_BYTE_AUTO_DECISIONS, COMMAND_STOP);
			
			level = WAIT_FOR_ITEM;
			COMMON_TOGGLE_PIN(PORTA, PORTA3);
			COMMON_TOGGLE_PIN(PORTA, PORTA1);
		}
		else if (level == GO_TO_GOAL)
		{
			level = DROP_ITEM;
		}
		else if(level == RETURN_AFTER_DELIVERED)
		{
			//level = FINISHED;
			
			Init_button_deliver();
			COMMON_TOGGLE_PIN(PORTA, PORTA3);
			COMMON_TOGGLE_PIN(PORTA, PORTA1);
			level = WAIT_FOR_ITEM;
		}
		else
		{
			following_path = FALSE;
			for(int i = 0; i < 4; i++)
			{
				if(p_robot_node->links[i].open == TRUE && p_robot_node->links[i].length == 0 && !turn_done)
				{
					Do_turn(i);
					turn_done = TRUE;
					//break;
					
				}
			}	
		}
	}
	else // Korsning är inte vårt mål.
	{
		Do_turn(Get_dijkpointers_cardinal_direction(p_robot_node)); // Do_turn enligt Dijkstra
	}
}

uint8_t Get_new_x_coordinate(uint8_t length)
{
	uint8_t x_coordinate = p_robot_node->x_coordinate;
	uint8_t traveled_blocks = Number_of_traveled_blocks(length); // Antalet färdade rutor
	
	switch (robot_dir)
	{
		case EAST:
		{
			x_coordinate = x_coordinate + traveled_blocks;
			break;
		}
		case WEST:
		{
			x_coordinate = x_coordinate - traveled_blocks;
			break;
		}
		default:
		break;
	}
	return x_coordinate;
}

uint8_t Get_new_y_coordinate(uint8_t length)
{
	uint8_t y_coordinate = p_robot_node->y_coordinate;
	uint8_t traveled_blocks = Number_of_traveled_blocks(length); // Antalet färdade rutor
	
	switch (robot_dir)
	{
		case NORTH:
		{
			y_coordinate = y_coordinate + traveled_blocks;
			break;
		}
		case SOUTH:
		{
			y_coordinate = y_coordinate - traveled_blocks;
			break;
		}
		default:
		break;
	}
	return y_coordinate;
}

uint8_t Number_of_traveled_blocks(uint8_t length)
{
	return (length + HALF_SIZE_OF_SQUARE_IN_TICS) / SIZE_OF_SQUARE_IN_TICS; // Ev. plussa något till length som marginal vid behov
}

//Följer pekarna
void Follow(uint8_t sensor_front, uint8_t sensor_front_left, uint8_t sensor_front_right, uint8_t sensor_back_left, uint8_t sensor_back_right)
{
	if(Crossing(sensor_front, sensor_front_left, sensor_front_right, sensor_back_left, sensor_back_right)) // Korsning?
	{
		if (enable_node_editing)
		{
			p_robot_node = p_robot_node->p_pre_dijk;
			if (p_robot_node == p_goal_node && !Is_node_crossing(p_robot_node))
			{
				p_robot_node = p_robot_node->p_pre_dijk; // Om vi kör över målet i en korridor, skippa även denna nod.
			}
			enable_node_editing = FALSE;
			length = Get_length();
			Following_path_at_crossing();
		}
		enable_safety = 0;
	}
	else // Inte en korsing och following_path == TRUE
	{
		Set_enable();
		//enable_node_editing = TRUE;
		//SPI_map_send_command_to_steering(ID_BYTE_AUTO_DECISIONS, COMMAND_FORWARD);
	}
}

void Search(uint8_t sensor_front, uint8_t sensor_front_left, uint8_t sensor_front_right, uint8_t sensor_back_left, uint8_t sensor_back_right)
{
	if (Crossing(sensor_front, sensor_front_left, sensor_front_right, sensor_back_left, sensor_back_right))
	{
		if (enable_node_editing)
		{
			length += Get_length();
			uint8_t	new_x_coordinate = Get_new_x_coordinate(length);
			uint8_t new_y_coordinate = Get_new_y_coordinate(length);
			
			node* p_arrived_node = Exisiting_node_at(new_x_coordinate, new_y_coordinate);
			if (p_arrived_node != NULL) // Noden finns redan
			{
				if (p_robot_node->x_coordinate != p_arrived_node->x_coordinate || p_robot_node->y_coordinate != p_arrived_node->y_coordinate)
				{
					Update_node(p_arrived_node, Number_of_traveled_blocks(length));
				}
				Find_shortest_path(Easy_find_unexplored_node(), p_robot_node);
				following_path = TRUE;
				Following_path_at_crossing();
			}
			else // Ej befintlig nod
			{
				Create_node(new_x_coordinate, new_y_coordinate, Number_of_traveled_blocks(length), What_is_open(sensor_front_left, sensor_front_right, sensor_front));
				//if (sensor_front < FRONT_SENSOR_LIMIT && sensor_front_left < SIDE_SENSOR_OPEN_LIMIT && sensor_front_right < SIDE_SENSOR_OPEN_LIMIT && sensor_back_left < SIDE_SENSOR_OPEN_LIMIT && sensor_back_right < SIDE_SENSOR_OPEN_LIMIT) // Återvändsgränd
				if (sensor_front < FRONT_SENSOR_LIMIT && sensor_front_left < SIDE_SENSOR_OPEN_LIMIT && sensor_front_right < SIDE_SENSOR_OPEN_LIMIT && sensor_back_left < SIDE_SENSOR_OPEN_LIMIT && sensor_back_right < SIDE_SENSOR_OPEN_LIMIT) // Återvändsgränd
				{
					enable_node_editing = TRUE;
					Find_shortest_path(Easy_find_unexplored_node(), p_robot_node);
					following_path = TRUE;
					Following_path_at_crossing();
				}
				else // Ej återvändsgränd och ny nod
				{
					if (sensor_front > SIZE_OF_SQUARE_IN_CM)
					{
						//Do_turn(robot_dir);
						length = 2;
					}
					else if (sensor_front_right > SIDE_SENSOR_OPEN_LIMIT)
					{
						Do_turn((robot_dir + 1) % 4);
					}
					else if (sensor_front_left > SIDE_SENSOR_OPEN_LIMIT)
					{
						Do_turn((robot_dir + 3) % 4);
					}
					else // Borde ej inträffa!
					{
						SPI_map_send_command_to_steering(ID_BYTE_AUTO_DECISIONS, COMMAND_STOP_6);
						level = FIRST_WAIT;
					}
				}
			}
		}
		else // Not enable node editing i korsning
		{
			//SPI_map_send_command_to_steering(ID_BYTE_AUTO_DECISIONS, COMMAND_FORWARD);
		}
		enable_safety = 0;
	}
	else // Ej korsning, ej following path
	{
		Set_enable();
		//enable_node_editing = TRUE;
		//SPI_map_send_command_to_steering(ID_BYTE_AUTO_DECISIONS, COMMAND_FORWARD);
	}
}

void Do_level_1(uint8_t sensor_front, uint8_t sensor_front_left, uint8_t sensor_front_right, uint8_t sensor_back_left, uint8_t sensor_back_right)
{
	if(following_path)
	{
		Follow(sensor_front, sensor_front_left, sensor_front_right, sensor_back_left, sensor_back_right);
	}
	else // following path == FALSE
	{
		Search(sensor_front, sensor_front_left, sensor_front_right, sensor_back_left, sensor_back_right);
	}
}

void Do_level_2(uint8_t sensor_front, uint8_t sensor_front_left, uint8_t sensor_front_right, uint8_t sensor_back_left, uint8_t sensor_back_right)
{
	if(following_path)
	{
		Follow(sensor_front, sensor_front_left, sensor_front_right, sensor_back_left, sensor_back_right);
	}
	else // following path == FALSE
	{
		Search_on_level_2(sensor_front, sensor_front_left, sensor_front_right, sensor_back_left, sensor_back_right);
	}
}


void Update_map(uint8_t sensor_front, uint8_t sensor_front_left, uint8_t sensor_front_right, uint8_t sensor_back_left, uint8_t sensor_back_right)
{
	if (SPI_map_should_handle_RFID())
	{
		Do_this_when_rfid_found(sensor_front_left, sensor_front_right, sensor_front);
	}
	//level = 1;
	switch(level)
	{
		case FIRST_WAIT: // avbrott på grå knapp
		{
			SPI_map_send_command_to_steering(ID_BYTE_AUTO_DECISIONS, COMMAND_STOP);
			break;
		}
		case SEARCH_FOR_GOAL: // RFID-tagg-avbrott, även till 42 om inte hittat tagg.
		{
			Do_level_1(sensor_front, sensor_front_left, sensor_front_right, sensor_back_left, sensor_back_right);
			break;
		}
		case KEEP_SEARCHING: // i funktionen Easy_find_unexplored
		{
			if(Easy_find_unexplored_node() == p_start_node)
			{
				level = RETURN_AFTER_FOUND;
			}
			else
			{
				Do_level_2(sensor_front, sensor_front_left, sensor_front_right, sensor_back_left, sensor_back_right);
			}
			break;
		}
		case RETURN_AFTER_FOUND: // i funktionen Follow_path_at_crossing
		{
			Follow(sensor_front, sensor_front_left, sensor_front_right, sensor_back_left, sensor_back_right);
			break;
		}
		case WAIT_FOR_ITEM: // Avbrott på svart knapp
		{
			SPI_map_send_command_to_steering(ID_BYTE_AUTO_DECISIONS, COMMAND_STOP);
			break;
		}
		
		case GRAB_ITEM_AND_CALC_DIJK: // i case:en
		{
			Find_shortest_path(p_goal_node, p_start_node);
			SPI_map_send_command_to_steering(ID_BYTE_AUTO_DECISIONS, COMMAND_CLOSE_CLAW);
			_delay_ms(500);
			Do_turn((robot_dir + 2) % 4);
			level = GO_TO_GOAL;
			SPI_map_send_command_to_steering(ID_BYTE_AUTO_DECISIONS, COMMAND_FORWARD);
			break;
		}
		case GO_TO_GOAL: // i funktionen Follow_path_at_crossing
		{
			if(Is_node_crossing(p_goal_node))
			{
				Follow(sensor_front, sensor_front_left, sensor_front_right, sensor_back_left, sensor_back_right);
			}
			else
			{
				if(p_robot_node->p_pre_dijk->p_pre_dijk != NULL)
				{
					Follow(sensor_front, sensor_front_left, sensor_front_right, sensor_back_left, sensor_back_right);
				}
				else
				{
					
					SPI_map_send_number_of_reflex_count_to_RFID_to_sensor(abs(p_robot_node->x_coordinate - p_goal_node->x_coordinate + p_robot_node->y_coordinate - p_goal_node->y_coordinate) * SIZE_OF_SQUARE_IN_TICS - 2);//abs(p_robot_node->x_coordinate - p_goal_node->x_coordinate + p_robot_node->y_coordinate - p_goal_node->y_coordinate) * SIZE_OF_SQUARE_IN_CM / 4.9);
					SPI_map_should_handle_reached_RFID();
					while (SPI_map_should_handle_reached_RFID() == FALSE)
					{
						//_delay_ms(5);
					}
					p_robot_node = p_goal_node;
					SPI_map_send_command_to_steering(ID_BYTE_AUTO_DECISIONS, COMMAND_STOP);
					level = DROP_ITEM;
				}
			}
			
			break;
		}
		case DROP_ITEM: // i case:en
		{
			
			SPI_map_send_command_to_steering(ID_BYTE_AUTO_DECISIONS,COMMAND_STOP);
			_delay_ms(100);
			SPI_map_send_command_to_steering(ID_BYTE_AUTO_DECISIONS,COMMAND_OPEN_CLAW);
			_delay_ms(100);
			
			SPI_map_send_command_to_steering(ID_BYTE_AUTO_DECISIONS,COMMAND_BACKWARD);
			_delay_ms(150);
			SPI_map_send_command_to_steering(ID_BYTE_AUTO_DECISIONS,COMMAND_STOP);
			_delay_ms(100);
			
			Do_turn((robot_dir + 2) % 4);
			
			//Find_shortest_path(p_robot_node, p_robot_node);
			//SPI_map_send_command_to_steering(ID_BYTE_AUTO_DECISIONS,COMMAND_FORWARD);
			
			
			Find_shortest_path(p_start_node, p_goal_node);
			SPI_map_send_command_to_steering(ID_BYTE_AUTO_DECISIONS,COMMAND_FORWARD);
			level = RETURN_AFTER_DELIVERED;
			break;
		}
		case RETURN_AFTER_DELIVERED:
		{
			Follow(sensor_front, sensor_front_left, sensor_front_right, sensor_back_left, sensor_back_right);
			break;		
		}
		case FINISHED:
		{
			SPI_map_send_command_to_steering(ID_BYTE_AUTO_DECISIONS,COMMAND_STOP);
			break;
		}
		// Test så länge RFID inte klar
		case 42: // Åk hem
		{
			//SPI_map_send_command_to_steering(ID_BYTE_AUTO_DECISIONS, COMMAND_STOP);
			Follow(sensor_front, sensor_front_left, sensor_front_right, sensor_back_left, sensor_back_right);
			break;
		}
		case 43: // Står stilla
		{
			SPI_map_send_command_to_steering(ID_BYTE_AUTO_DECISIONS, COMMAND_STOP);
			break;
		}
		default:
		SPI_map_send_command_to_steering(ID_BYTE_AUTO_DECISIONS, COMMAND_STOP_5);
		_delay_ms(2000);
		break;	
	}
	Map_send_map_parameters_to_PC(robot_dir, all_nodes_size, following_path, enable_node_editing, level, p_robot_node->x_coordinate, p_robot_node->y_coordinate);
}

// Koden anropas då rfid hittas och antingen uppdaterar redan befintlig nod eller skapar en ny nod som goal.
void Do_this_when_rfid_found(uint8_t sensor_front_left, uint8_t sensor_front_right, uint8_t sensor_front)
{
	if (level == SEARCH_FOR_GOAL || level == 42)
	{
		SPI_map_send_command_to_steering(ID_BYTE_AUTO_DECISIONS, COMMAND_STOP_1);
		_delay_ms(1000);
		
		length += Get_length();
		uint8_t	new_x_coordinate = Get_new_x_coordinate(length);
		uint8_t new_y_coordinate = Get_new_y_coordinate(length);
		
		node* p_arrived_node = Exisiting_node_at(new_x_coordinate, new_y_coordinate);
		if (p_arrived_node != NULL) // Vi har hittat RFIDn efter att vi har skapat noden
		{
			p_robot_node->goal = TRUE;
			p_goal_node = p_robot_node;
			// [nått mer här]
		}
		else // Vi skapar målnod
		{
			Create_goal(new_x_coordinate, new_y_coordinate, Number_of_traveled_blocks(length), What_is_open(sensor_front_left, sensor_front_right, sensor_front));
		}
		
		
		/*
		if (!enable_node_editing)
		{
			p_robot_node->goal = TRUE;
		}
		else
		{
			uint8_t length = Get_length();
			Create_goal(Get_new_x_coordinate(length), Get_new_y_coordinate(length), Number_of_traveled_blocks(length), What_is_open(sensor_front_left, sensor_front_right, sensor_front));
		}
		*/
		COMMON_TOGGLE_PIN(PORTA, PORTA3);
		length = 0;
		level = KEEP_SEARCHING;
		SPI_map_send_command_to_steering(ID_BYTE_AUTO_DECISIONS, COMMAND_FORWARD);
	}
	if (level == GO_TO_GOAL)
	{
		SPI_map_send_command_to_steering(ID_BYTE_AUTO_DECISIONS, COMMAND_STOP);
		level = DROP_ITEM;
	}
}

void Set_enable()
{
	if(enable_safety > 1)
	{
		enable_node_editing = 1;
		enable_safety = 0;
	}
	else
	{
		enable_safety++;
	}
}

//Returnerar sant om noden befinner sig i en korsning, falskt om inte.
uint8_t Is_node_crossing(node* p_node)
{
	if ((p_node->links[0].open + p_node->links[1].open + p_node->links[2].open + p_node->links[3].open) == 2 )
	{
		uint8_t buffer = 0;
		for(int i = 0 ; i < 4; i++)
		{
			if(p_node->links[i].open == TRUE)
			{
				buffer = buffer + i;
			}
		}
		return buffer % 2;
	}
	else
	{
		return TRUE;
	}
}

node* Find_unexplored_node()
{
	for (int i = all_nodes_size - 1; i > 0; i--)
	{
		for (int n = 0; n < NUMBER_OF_LINKS; n++)
		{
			if (all_nodes[i]->links[n].open && all_nodes[i]->links[n].length == 0) // Om öppen med längd 0, alltså outforskad.
			{
				uint8_t so_far_best_way = Find_shortest_path(p_goal_node, p_start_node);
				uint8_t potential_better_way = Find_shortest_path(p_goal_node, all_nodes[i]) + abs(all_nodes[i]->x_coordinate - 30) + abs(all_nodes[i]->y_coordinate - 30);
				if(potential_better_way < so_far_best_way)
				{
				return all_nodes[i];
				}
			}
		}
	}
	if(level == KEEP_SEARCHING)
	{
		Find_shortest_path(p_start_node, p_robot_node);
		following_path = TRUE;
		level = RETURN_AFTER_FOUND; // Level up 2->3
	}
	if(level == SEARCH_FOR_GOAL)
	{
		Find_shortest_path(p_start_node, p_robot_node);
		following_path = TRUE;
		level = 42; // Om Lassie inte hittat RFID
		SPI_map_send_command_to_steering(ID_BYTE_AUTO_DECISIONS, COMMAND_FORWARD);
	}
	return all_nodes[0]; // åker till start om upptäckt hela, levlar innan åker hem
}

void Search_on_level_2(uint8_t sensor_front, uint8_t sensor_front_left, uint8_t sensor_front_right, uint8_t sensor_back_left, uint8_t sensor_back_right)
{
	if (Crossing(sensor_front, sensor_front_left, sensor_front_right, sensor_back_left, sensor_back_right))
	{
		if (enable_node_editing)
		{
			length += Get_length();
			uint8_t	new_x_coordinate = Get_new_x_coordinate(length);
			uint8_t new_y_coordinate = Get_new_y_coordinate(length);
			
			node* p_arrived_node = Exisiting_node_at(new_x_coordinate, new_y_coordinate);
			if (p_arrived_node != NULL) // Noden finns redan
			{
				if (p_robot_node->x_coordinate != p_arrived_node->x_coordinate || p_robot_node->y_coordinate != p_arrived_node->y_coordinate)
				{
					Update_node(p_arrived_node, Number_of_traveled_blocks(length));
				}
				Find_shortest_path(Find_unexplored_node(), p_robot_node);
				following_path = TRUE;
				Following_path_at_crossing();
			}
			else // Ej befintlig nod
			{
				Create_node(new_x_coordinate, new_y_coordinate, Number_of_traveled_blocks(length), What_is_open(sensor_front_left, sensor_front_right, sensor_front));
				//if (sensor_front < FRONT_SENSOR_LIMIT && sensor_front_left < SIDE_SENSOR_OPEN_LIMIT && sensor_front_right < SIDE_SENSOR_OPEN_LIMIT && sensor_back_left < SIDE_SENSOR_OPEN_LIMIT && sensor_back_right < SIDE_SENSOR_OPEN_LIMIT) // Återvändsgränd
				if (sensor_front < FRONT_SENSOR_LIMIT && sensor_front_left < SIDE_SENSOR_OPEN_LIMIT && sensor_front_right < SIDE_SENSOR_OPEN_LIMIT && sensor_back_left < SIDE_SENSOR_OPEN_LIMIT && sensor_back_right < SIDE_SENSOR_OPEN_LIMIT) // Återvändsgränd
				{
					enable_node_editing = TRUE;
					Find_shortest_path(Find_unexplored_node(), p_robot_node);
					following_path = TRUE;
					Following_path_at_crossing();
				}
				else // Ej återvändsgränd och ny nod
				{
					if (sensor_front > SIZE_OF_SQUARE_IN_CM)
					{
						//Do_turn(robot_dir);
						length = 2;
					}
					else if (sensor_front_right > SIDE_SENSOR_OPEN_LIMIT)
					{
						Do_turn((robot_dir + 1) % 4);
					}
					else if (sensor_front_left > SIDE_SENSOR_OPEN_LIMIT)
					{
						Do_turn((robot_dir + 3) % 4);
					}
					else // Borde ej inträffa!
					{
						SPI_map_send_command_to_steering(ID_BYTE_AUTO_DECISIONS, COMMAND_STOP_6);
						level = FIRST_WAIT;
					}
				}
			}
		}
		else // Not enable node editing i korsning
		{
			//SPI_map_send_command_to_steering(ID_BYTE_AUTO_DECISIONS, COMMAND_FORWARD);
		}
		enable_safety = 0;
	}
	else // Ej korsning, ej following path
	{
		Set_enable();
		//enable_node_editing = TRUE;
		//SPI_map_send_command_to_steering(ID_BYTE_AUTO_DECISIONS, COMMAND_FORWARD);
	}
}