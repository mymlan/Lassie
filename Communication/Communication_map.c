#define F_CPU 18432000UL

#include <avr/io.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include "../CommonLibrary/Common.h"
#include "Communication_map.h"
#include "Communication_SPI.h"
#include "Firefly.h"
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
		if(left > SIDE_SENSOR_OPEN_LIMIT)
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
	return ERROR_IN_WHAT_IS_OPEN; //något har gått fel // man returnerar egentligen bara 0 när att gått bra. Andra siffror betyrer vissa felmeddelanden
}

// Create_origin
// Funktionen skapar en origonod som den returnerar en pekare till (Det är tänkt att robot_node sätts till detta returvärde när den skapas)
void Create_origin(uint8_t open_walls)
{
	// Funktionen kan ändras beroende på hur start ser ut i labyrinten
	node* p_node = Newnode(30, 0); // Skapa nod
	
	p_node->start = TRUE; // Sätt nod till startnod
	
	for(uint8_t i = 0; i < NUMBER_OF_LINKS; i++) // Detta kanske ändras eller tas bort
	{
		p_node->links[i].open = ((open_walls >> (3 - i)) & 0x01); // Sätt rätt .open till TRUE
	}
	p_robot_node = p_node;
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
	for(uint8_t i = 0; i < NUMBER_OF_LINKS; i++)
	{
		p_node->links[i].open = ((open_walls >> (3 - i)) & 0x01); // 3a. Sätt rätt .open till TRUE
	}
	p_node->links[(robot_dir + 2) % NUMBER_OF_LINKS].p_node = p_robot_node; // 3b. Koppla gammal nod till ny nod
	p_node->links[(robot_dir + 2) % NUMBER_OF_LINKS].length = length; // 3c. Längd för vägen bakåt satt
	p_robot_node = p_node;// 4.
	// 5. Nollställ avstånd (kanske sker utanför funktionen)
	// 6. Ta styrbslut och return (sker utanför funktionen)
	enable_node_editing = FALSE;
}

// Create_goal
// Funktionen skapar en målnod som den returnerar en pekare till (Det är tänkt att robot_node sätts till detta returvärde när den skapas)
void Create_goal(uint8_t x_coordinate, uint8_t y_coordinate,uint8_t length, uint8_t open_walls)
{
	// Funktionen kan ändras beroende på hur start ser ut i labyrinten
	Create_node(x_coordinate, y_coordinate, length, open_walls); // Skapa nod
	p_robot_node->goal = TRUE; // Sätt nod till startnod
	goal_node = p_robot_node;
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
	
	p_robot_node = p_node;
	
	enable_node_editing = FALSE;
}

// Existing_node_at
// Funktionen returnerar pekare till noden som finns på de givna koordinaterna om det finnsen nod där. Annars returneras NULL;
node* Exisiting_node_at(uint8_t x_coordinate, uint8_t y_coordinate)
{
	for (uint8_t i = 0; i < all_nodes_size; i++)
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
	for (uint8_t i = 0; i < all_nodes_size ; i++)
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
	for(uint8_t i = 0; i < all_nodes_size; i++)
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
	for(uint8_t i = 0; i < all_nodes_size; i++)
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
	
	while(p_node2->searched == FALSE)
	{
		//2. Hitta den nod som har lägst nodpris, första gången startnoden.
		node* p_chosen_node = all_nodes[Find_low_cost_index()];
		//3. Ge angränsande noder uppdaterade värden om deras kostnad och föregångare.
		for(uint8_t n = 0; n < NUMBER_OF_LINKS; n++)
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
}

// Do_turn
// Funktonen utför en sväng eller liknande för att rotera roboten i given riktning genom anrop till styr och sensormodulerna
void Do_turn(uint8_t cardinal_direction)
{
	SPI_map_send_command_to_steering(ID_BYTE_AUTO_DECISIONS, COMMAND_STOP_1);
	_delay_ms(1000);
	switch ((robot_dir - cardinal_direction + NUMBER_OF_LINKS) % NUMBER_OF_LINKS)
	{
		case 3:
		{
			// Turn right order
			SPI_map_send_id_byte_to_sensor(ID_BYTE_START_ANGULAR_RATE_SENSOR);
			SPI_map_send_command_to_steering(ID_BYTE_AUTO_DECISIONS, COMMAND_TIGHT_TURN_RIGHT);
			Map_send_byte_to_PC(ID_BYTE_AUTO_DECISIONS, COMMAND_TIGHT_TURN_RIGHT);
			Wait_for_90_degree_rotation();
			
			robot_dir = (robot_dir + 1) % NUMBER_OF_LINKS;
			break;
		}
		case 2:
		{
			// Rotation 180 degrees order
			SPI_map_send_id_byte_to_sensor(ID_BYTE_START_ANGULAR_RATE_SENSOR);
			SPI_map_send_command_to_steering(ID_BYTE_AUTO_DECISIONS, COMMAND_ROTATE_RIGHT);
			Map_send_byte_to_PC(ID_BYTE_AUTO_DECISIONS, COMMAND_ROTATE_RIGHT);
			Wait_for_90_degree_rotation(); // 90 grader
			SPI_map_send_id_byte_to_sensor(ID_BYTE_START_ANGULAR_RATE_SENSOR);
			SPI_map_send_command_to_steering(ID_BYTE_AUTO_DECISIONS, COMMAND_ROTATE_RIGHT);
			Wait_for_90_degree_rotation(); // ytterligare 90 grader
			
			robot_dir = (robot_dir + 2) % NUMBER_OF_LINKS;
			break;
		}
		case 1:
		{
			// Turn left order
			SPI_map_send_id_byte_to_sensor(ID_BYTE_START_ANGULAR_RATE_SENSOR);
			SPI_map_send_command_to_steering(ID_BYTE_AUTO_DECISIONS,COMMAND_TIGHT_TURN_LEFT);
			Map_send_byte_to_PC(ID_BYTE_AUTO_DECISIONS, COMMAND_TIGHT_TURN_LEFT);
			Wait_for_90_degree_rotation();
			
			robot_dir = (robot_dir + 3) % NUMBER_OF_LINKS;
			break;
		}
		case 0:
		{
			break;
		}
		default:
		break;
	}
	SPI_map_send_command_to_steering(ID_BYTE_AUTO_DECISIONS, COMMAND_STOP_2);
	_delay_ms(1000);
	// Åk fram oreglerat order
	SPI_map_send_command_to_steering(ID_BYTE_AUTO_DECISIONS, COMMAND_FORWARD); // Not regulated kanske
	Map_send_byte_to_PC(ID_BYTE_AUTO_DECISIONS, COMMAND_FORWARD);
}

// Get_dijkpointers_cardinal_direction
// Funktionen tar in en nodpekare och returnerar dess dijkstraspekares vädersträck
uint8_t Get_dijkpointers_cardinal_direction(node* p_node)
{
	for (uint8_t i = 0; i < NUMBER_OF_LINKS; i++)
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
		for (uint8_t i = 0; i < NUMBER_OF_LINKS; i++)
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
	for(int16_t i = all_nodes_size - 1; i > 0; i--)
	{
		for (uint8_t n = 0; n < NUMBER_OF_LINKS; n++)
		{
			if (all_nodes[i]->links[n].open && all_nodes[i]->links[n].length == 0) // Om öppen med längd 0, alltså outforskad.
			{
				return all_nodes[i];
			}
		}
	}
	if(level == 1)
	{
		level++; // Level up 1->2	
	}
	return all_nodes[0]; // åker till start om upptäckt hela
}

//En smartare variant av Easy_find_unexplored_node
//Hittar istället den nod som är minst antal steg från given nod(oftast robotpekaren som anropas)
node* Smarter_find_unexplored_node(node* p_node)
{
	for(uint8_t i = 0; i < NUMBER_OF_LINKS; i++) //kolar om någon link är outforskad
	{
		if(p_node->links[i].open && p_node->links[i].length == 0) // öppen och längd 0 -> outforskad väg
		{
			return p_node;
		}
	}
	for(uint8_t n = 0; n < NUMBER_OF_LINKS; n++) //Om inte hittad outforskad, gör samma sak på grannarna
	{
		if(p_node->links[n].p_node != NULL && !p_node->links[n].p_node->start) //Om nästa nod inte är NULL och inte startnod
		{
			return Smarter_find_unexplored_node(p_node->links[n].p_node); //Så ska genomsökning på den noden ske.
		}
	}
	level++; // Level up 1->2
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

// Search
// Funktionen tar styrbeslut för sökning
void Search(uint8_t sensor_front, uint8_t sensor_front_left, uint8_t sensor_front_right, uint8_t sensor_back_left, uint8_t sensor_back_right)
{
	int length;
	// -------- Detta är kartlagring och genomsökning ---------- //
	// Indikerar sensorvärden återvändsgränd?
	if (sensor_front < FRONT_SENSOR_LIMIT && sensor_front_left < SIDE_SENSOR_OPEN_LIMIT && sensor_front_right < SIDE_SENSOR_OPEN_LIMIT) // 10 cm så vi kommer nära väggen och klarar detektera en ev. RFID
	{
		// UPPDATERA LENGTH! Fixas vid anrop till sensormodul  (2 STÄLLEN)
		//SPI_map_send_command_to_steering(ID_BYTE_AUTO_DECISIONS, COMMAND_STOP_1);
		_delay_ms(1);
		length = Get_length();
		SPI_map_send_command_to_steering(ID_BYTE_AUTO_DECISIONS, COMMAND_FORWARD);
		_delay_ms(1);
		// Uträkning av koordinater
		uint8_t x_coordinate = p_robot_node->x_coordinate;
		uint8_t y_coordinate = p_robot_node->y_coordinate;
		traveled_blocks = (length + TRAVELED_BLOCKS_MARGIN) / SIZE_OF_SQUARE_IN_CM; // Antalet färdade rutor
		switch (robot_dir)
		{
			case NORTH:
			{
				y_coordinate = y_coordinate + traveled_blocks;
				break;
			}
			case EAST:
			{
				x_coordinate = x_coordinate + traveled_blocks;
				break;
			}
			case SOUTH:
			{
				y_coordinate = y_coordinate - traveled_blocks;
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
		// Skapa nod
		Create_node(x_coordinate, y_coordinate, traveled_blocks, What_is_open(sensor_front_left, sensor_front_right, sensor_front));
		// Åk utforskad väg till närmsta outforskade nod
		Find_shortest_path(Easy_find_unexplored_node(), p_robot_node);
		following_path = 1;
		Follow_path();
		// robot_dir = (robot_dir + 2) % 4; // antagligen ska bort
	}
	// Annars, indikerar bakre sensorer en korsning?
	else if (sensor_back_left > SIDE_SENSOR_OPEN_LIMIT || sensor_back_right > SIDE_SENSOR_OPEN_LIMIT)
	{
		// UPPDATERA LENGTH! Fixas vid anrop till sensormodul (2 STÄLLEN)
		//SPI_map_send_command_to_steering(ID_BYTE_AUTO_DECISIONS, COMMAND_STOP_2);
		_delay_ms(1);
		length = Get_length();
		SPI_map_send_command_to_steering(ID_BYTE_AUTO_DECISIONS, COMMAND_FORWARD);
		_delay_ms(1);
		// Uträkning av koordinater
		uint8_t x_coordinate = p_robot_node->x_coordinate;
		uint8_t y_coordinate = p_robot_node->y_coordinate;
		traveled_blocks = (length + TRAVELED_BLOCKS_MARGIN) / SIZE_OF_SQUARE_IN_CM; // Antalet färdade rutor
		switch (robot_dir)
		{
			case NORTH:
			{
				y_coordinate = y_coordinate + traveled_blocks;
				break;
			}
			case EAST:
			{
				x_coordinate = x_coordinate + traveled_blocks;
				break;
			}
			case SOUTH:
			{
				y_coordinate = y_coordinate - traveled_blocks;
				break;
			}
			case WEST:
			{
				x_coordinate = x_coordinate - traveled_blocks;
				break;
			}
		}
		// Är det en ny korsning?
		if (Exisiting_node_at(x_coordinate, y_coordinate) == NULL)
		{
			// Skapa nod utifrån främre 3 sensorer
			Create_node(x_coordinate, y_coordinate, traveled_blocks, What_is_open(sensor_front_left, sensor_front_right, sensor_front));
			// Öppet rakt fram?
			// Ingen förändring av styrbeslut
			if (sensor_front > SIZE_OF_SQUARE_IN_CM)
			{
				// Ändra inte robot_dir
				Do_turn(robot_dir);
			}
			// Annars, öppet höger?
			else if (sensor_front_right > SIDE_SENSOR_OPEN_LIMIT)
			{
				// Ge order om sväng höger
				Do_turn((robot_dir + 1) % NUMBER_OF_LINKS);
				//robot_dir = (robot_dir + 1) % NUMBER_OF_LINKS; // Ska antagligen tas bort
			}
			// Annars (öppet vänster)
			else
			{
				// Ge order om svänga vänster
				Do_turn((robot_dir + 3) % NUMBER_OF_LINKS);
				//robot_dir = (robot_dir + 3) % NUMBER_OF_LINKS; // Ska antagligen tas bort
			}
		}
		// Annars (gammal korsning)
		else
		{
			// Uppdatera nod
			c = x;
			d = y;
			e = x_coordinate;
			f = y_coordinate;
			Update_node(Exisiting_node_at(x_coordinate, y_coordinate), traveled_blocks);
			// Åk utforskad väg till närmsta outforskade nod
			Find_shortest_path(Easy_find_unexplored_node(), p_robot_node);
			following_path = TRUE;
			Follow_path();
		}
	}
	Map_send_map_parameters_to_PC(robot_dir, all_nodes_size, following_path, enable_node_editing, level, p_robot_node->x_coordinate, p_robot_node->y_coordinate);
}

// Sensor_values_has_arrived
// Denna kod körs varje gång sensorvärden kommer. Koden kan senare ev. flyttas till mainloopen när allt fungerar.
// När sensorvärden kommer, kör denna kod. Koden avgör om man är i en korsning och beroende på om det är en ny eller gammal korsning skapas eller uppdateras noden.
// Det finns avsatta rader där strybeslut skickas till styrmodulen.
void Update_map_old(uint8_t sensor_front, uint8_t sensor_front_left, uint8_t sensor_front_right, uint8_t sensor_back_left, uint8_t sensor_back_right)
{
	//enable_node_editing = TRUE; // <----- Ska tas bort vid riktiga körningar!!!!!!!!!!!!!!!!!!!!!
	
	if(sensor_back_left < SIDE_SENSOR_OPEN_LIMIT && sensor_back_right < SIDE_SENSOR_OPEN_LIMIT && sensor_front_left < SIDE_SENSOR_OPEN_LIMIT && sensor_front_right < SIDE_SENSOR_OPEN_LIMIT)// hade räckt med bakre, men säkrare att kolla alla.
	{
		enable_node_editing = TRUE;
		if (following_path && p_robot_node->p_pre_dijk == NULL)
		{
			following_path = FALSE;
		}
		/*
		if (start_regulating)
		{
			SPI_map_send_command_to_steering(ID_BYTE_AUTO_DECISIONS, COMMAND_FORWARD);
			start_regulating = FALSE;
		}
		*/
		
		//if (TRUE) // SKA ÄNDRAS när vi börjar levla upp. Kan ge oönskade åk rakt fram ordrar. // Ska lösa problemet med reglering i korsning men är ej klart.
		//{
		//	SPI_map_send_command_to_steering(ID_BYTE_AUTO_DECISIONS, COMMAND_FORWARD);
		//}
	}
	
	if (following_path)
	{
		// Korsning?
		if (sensor_back_left > SIDE_SENSOR_OPEN_LIMIT || sensor_back_right > SIDE_SENSOR_OPEN_LIMIT || p_robot_node->p_pre_dijk->start)
		{
			// Får vi ändra nod?
			if(enable_node_editing)
			{
				p_robot_node = p_robot_node->p_pre_dijk; // ska ju bara göras en gång per nod
				enable_node_editing = FALSE;
				if (p_robot_node->start && (level == 2 || level == 6))
				{
					level++; // Level 2->3, 6->7
				}
				else if (p_robot_node->goal && level == 4)
				{
					level++; // Level 4->5
				}
			}
			Follow_path(); // borde kunna köra flera ggr per nod, do_turn svänger inte flera gånger.
		}
	}
	else
	{
		switch(level)
		{
			case 0: // Lassie letare efter målet. [Level i avbrott]
			if (enable_node_editing)
			{
				Search(sensor_front, sensor_front_left, sensor_front_right, sensor_back_left, sensor_back_right);
			}
			break;
			
			case 1: // Fortsätter leta efter väg. Level i Easy_find_unexplored_node
			if (enable_node_editing)
			{
				Search(sensor_front, sensor_front_left, sensor_front_right, sensor_back_left, sensor_back_right);
			}
			break;
			
			case 2: // Lassie åker hem. Level i Sensor_values_has_arrived
			Find_shortest_path(all_nodes[0], p_robot_node);
			following_path = TRUE;
			Follow_path();
			break;
			
			case 3: // Väntar på proviant. [Level i avbrott från knapp]
			SPI_map_send_command_to_steering(ID_BYTE_AUTO_DECISIONS, COMMAND_STOP_3);
			
			case 4: // Åk till mål. Level i Sensor_values_has_arrived
			SPI_map_send_command_to_steering(ID_BYTE_AUTO_DECISIONS, COMMAND_CLOSE_CLAW);
			// [Kort delay]
			Find_shortest_path(goal_node, all_nodes[0]);
			following_path = TRUE;
			Follow_path();
			break;
			
			case 5: // Stanna, vänta, loss, levla. Level här.
			SPI_map_send_command_to_steering(ID_BYTE_AUTO_DECISIONS, COMMAND_STOP_4);
			// [Kort delay]
			SPI_map_send_command_to_steering(ID_BYTE_AUTO_DECISIONS, COMMAND_OPEN_CLAW);
			// [Kort delay]
			level++; // Ev. bugg till senare: Lassie ska åka samma väg tillbaka.
			break;
			
			case 6: // Åk hem. Level i Sensor_values_has_arrived.
			Find_shortest_path(all_nodes[0], p_robot_node);
			following_path = TRUE;
			Follow_path();
			break;
			
			case 7: // Inget. Vi är klara. Ingen levling.
			SPI_map_send_command_to_steering(ID_BYTE_AUTO_DECISIONS, COMMAND_STOP_5);
			break;
		}
	}
}

//initieringsfunktion, ska köras i början av huvudloopen i Communication.c
void Map_init()
{
	all_nodes_size = 0;
	following_path = 0;
	level = 0;
	enable_node_editing = 1;
	robot_dir = NORTH;
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
	if(p_robot_node->p_pre_dijk == NULL) // Är korsningen vårt mål?
	{
		following_path = FALSE;
		for(int i = 0; i < 4; i++)
		{
			if(p_robot_node->links[i].open == TRUE && p_robot_node->links[i].length == 0)
			{
				Do_turn(i);
				break;
					
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
	return (length + TRAVELED_BLOCKS_MARGIN) / SIZE_OF_SQUARE_IN_CM;
}

void Do_level_1(uint8_t sensor_front, uint8_t sensor_front_left, uint8_t sensor_front_right, uint8_t sensor_back_left, uint8_t sensor_back_right)
{
	uint8_t length;
	if(following_path)
	{
		if(Crossing(sensor_front, sensor_front_left, sensor_front_right, sensor_back_left, sensor_back_right)) // Korsning?
		{
			if (enable_node_editing)
			{
				p_robot_node = p_robot_node->p_pre_dijk;
				enable_node_editing = FALSE;
				length = Get_length();
			}
			Following_path_at_crossing();
		}
		else // Inte en korsing och following_path == TRUE
		{
			enable_node_editing = TRUE;
			//SPI_map_send_command_to_steering(ID_BYTE_AUTO_DECISIONS, COMMAND_FORWARD);
		}
	}
	else // following path == FALSE
	{
		if (Crossing(sensor_front, sensor_front_left, sensor_front_right, sensor_back_left, sensor_back_right))
		{
			if (enable_node_editing)
			{
				length = Get_length();
				node* p_arrived_node = Exisiting_node_at(Get_new_x_coordinate(length), Get_new_y_coordinate(length));
				if (p_arrived_node != NULL) // Noden finns redan
				{
					Update_node(p_arrived_node, Number_of_traveled_blocks(length));
					
					Find_shortest_path(Easy_find_unexplored_node(), p_robot_node);
					following_path = TRUE;
					Following_path_at_crossing();
				}
				else // Ej befintlig nod
				{
					Create_node(Get_new_x_coordinate(length), Get_new_y_coordinate(length), Number_of_traveled_blocks(length), What_is_open(sensor_front_left, sensor_front_right, sensor_front));
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
							Do_turn(robot_dir);
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
						}
					}
				}
			}
			else // Not enable node editing i korsning
			{
				//SPI_map_send_command_to_steering(ID_BYTE_AUTO_DECISIONS, COMMAND_FORWARD);
			}
		}
		else // Ej korsning, ej following path
		{
			enable_node_editing = TRUE;
			//SPI_map_send_command_to_steering(ID_BYTE_AUTO_DECISIONS, COMMAND_FORWARD);
		}
	}
}
// ev. funktion deleta allt allokerat minne mha free()
// (kanske inte behövs då vi inte ska deleta enskilda noder, och kan reseta minnet mellan körningar)
void Do_level_2(uint8_t sensor_front, uint8_t sensor_front_left, uint8_t sensor_front_right, uint8_t sensor_back_left, uint8_t sensor_back_right)
{
	uint8_t length;
	if(following_path)
	{
		if(Crossing(sensor_front, sensor_front_left, sensor_front_right, sensor_back_left, sensor_back_right)) // Korsning?
		{
			if (enable_node_editing)
			{
				p_robot_node = p_robot_node->p_pre_dijk;
				enable_node_editing = FALSE;
				length = Get_length();
			}
			Following_path_at_crossing();
		}
		else // Inte en korsing och following_path == TRUE
		{
			//SPI_map_send_command_to_steering(ID_BYTE_AUTO_DECISIONS, COMMAND_FORWARD);
		}
	}
	else // following path == FALSE
	{
		if (Crossing(sensor_front, sensor_front_left, sensor_front_right, sensor_back_left, sensor_back_right))
		{
			if (enable_node_editing)
			{
				length = Get_length();
				node* p_arrived_node = Exisiting_node_at(Get_new_x_coordinate(length), Get_new_y_coordinate(length));
				if (p_arrived_node != NULL) // Noden finns redan
				{
					Update_node(p_arrived_node, Number_of_traveled_blocks(length));
					
					Find_shortest_path(Easy_find_unexplored_node(), p_robot_node);
					following_path = TRUE;
					Following_path_at_crossing();
				}
				else // Ej befintlig nod
				{
					Create_node(Get_new_x_coordinate(length), Get_new_y_coordinate(length), Number_of_traveled_blocks(length), What_is_open(sensor_front_left, sensor_front_right, sensor_front));
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
							Do_turn(robot_dir);
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
						}
					}
				}
			}
			else // Not enable node editing i korsning
			{
				//SPI_map_send_command_to_steering(ID_BYTE_AUTO_DECISIONS, COMMAND_FORWARD);
			}
		}
		else // Ej korsning
		{
			enable_node_editing = TRUE;
			//SPI_map_send_command_to_steering(ID_BYTE_AUTO_DECISIONS, COMMAND_FORWARD);
		}
	}
}

void Update_map(uint8_t sensor_front, uint8_t sensor_front_left, uint8_t sensor_front_right, uint8_t sensor_back_left, uint8_t sensor_back_right)
{
	//level = 1;
	switch(level)
	{
		case 0:
		{
			SPI_map_send_command_to_steering(ID_BYTE_AUTO_DECISIONS, COMMAND_STOP);
			break;
		}
		case 1:
		{
			Do_level_1(sensor_front, sensor_front_left, sensor_front_right, sensor_back_left, sensor_back_right);
			break;
		}
		default:
		break;	
	}
	Map_send_map_parameters_to_PC(robot_dir, all_nodes_size, following_path, enable_node_editing, level, p_robot_node->x_coordinate, p_robot_node->y_coordinate);
}