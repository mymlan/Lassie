#include <avr/io.h>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h> // bool
#include "map_communication_functions.h"

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
	
	p_node->links[0].length = 0;
	p_node->links[1].length = 0;
	p_node->links[2].length = 0;
	p_node->links[3].length = 0;
	
	all_nodes[all_nodes_size] = p_node;
	all_nodes_size++;
	
	return p_node;
}