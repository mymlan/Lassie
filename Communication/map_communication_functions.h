#ifndef MAP_COMMUNIVATION_FUNCTIONS_H
#define MAP_COMMUNIVATION_FUNCTIONS_H

//------ C, VI LOVAR ATT DESSA STRUCTAR DEFINIERAS -------//
struct node_;
struct link_;
//typedef struct node_ node;
//typedef struct link_ link;

//------ C, VI LOVAR ATT DESSA FUNKTIONER DEFINIERAS, AKA INITIERING -------//
struct node_ *Newnode(uint8_t, uint8_t);
struct link_ *Newlink(uint8_t, struct node_*);

struct node_* all_nodes[200]; // Sparar alla noder
uint8_t all_nodes_size;
struct node_* p_robot_node;
uint8_t robot_dir;
struct node_* goal_node;

uint8_t following_path;
uint8_t level;
uint8_t enable_node_editing;

int test_variable_a;
int test_variable_b;
int a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z; // testvariabler

//int robot_dir = 0; // nord, öst, syd, väst


//-------------------- STRUCT-DEFINITIONER------------------------//
typedef struct link_
{
	uint8_t length;
	struct node_ *p_node;
	uint8_t open; // true om väg finns
} link;
typedef struct node_
{
	uint8_t x_coordinate;
	uint8_t y_coordinate;
	uint8_t cost;
	uint8_t searched;
	struct node_ *p_pre_dijk; // föregångare
	uint8_t start; // sant/falskt
	uint8_t goal; // sant/falskt
	link links[4]; // Jag tror det är lättare om den har fix storlek och så får vi ha en bool för varje link som avgör öppen eller ej
	// Det ska vara en fyra här (4 st element 0-3)
} node;

#endif