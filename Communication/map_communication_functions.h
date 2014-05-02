#ifndef MAP_COMMUNIVATION_FUNCTIONS_H
#define MAP_COMMUNIVATION_FUNCTIONS_H

//------ C, VI LOVAR ATT DESSA STRUCTAR DEFINIERAS -------//
struct node_;
struct link_;
//typedef struct node_ node;
//typedef struct link_ link;

//------ C, VI LOVAR ATT DESSA FUNKTIONER DEFINIERAS, AKA INITIERING -------//
struct node_ *Newnode(int, int);
struct link_ *Newlink(int, struct node_*);

struct node_* all_nodes[200]; // Sparar alla noder
uint8_t all_nodes_size;
struct node_* p_robot_node;
int robot_dir;

bool following_path, searching;

bool enable_node_editing;

int test_variable_a;
int test_variable_b;
int a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z; // testvariabler

//int robot_dir = 0; // nord, öst, syd, väst
int NORTH;
int EAST;
int SOUTH;
int WEST;


//-------------------- STRUCT-DEFINITIONER------------------------//
typedef struct link_
{
	int length;
	struct node_ *p_node;
	bool open; // true om väg finns
} link;
typedef struct node_
{
	int x;
	int y;
	int cost;
	bool searched;
	struct node_ *p_pre_dijk; // föregångare
	bool start; // sant/falskt
	bool goal; // sant/falskt
	link links[4]; // Jag tror det är lättare om den har fix storlek och så får vi ha en bool för varje link som avgör öppen eller ej
	// Det ska vara en fyra här (4 st element 0-3)
} node;

#endif