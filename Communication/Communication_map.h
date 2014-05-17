#ifndef _COMMUNICATION_MAP_H
#define _COMMUNICATION_MAP_H

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
struct node_* start_node;

uint8_t following_path;
uint8_t level;
uint8_t enable_node_editing;
uint8_t traveled_blocks;
uint8_t enable_safety;
uint8_t length;

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
	int8_t x_coordinate;
	int8_t y_coordinate;
	uint8_t cost;
	uint8_t searched;
	struct node_ *p_pre_dijk; // föregångare
	uint8_t start; // sant/falskt
	uint8_t goal; // sant/falskt
	link links[4]; // Jag tror det är lättare om den har fix storlek och så får vi ha en bool för varje link som avgör öppen eller ej
	// Det ska vara en fyra här (4 st element 0-3)
} node;


uint8_t What_is_open(uint8_t left, uint8_t right, uint8_t front);
void Create_origin(uint8_t open_walls);
void Create_node(uint8_t x_coordinate, uint8_t y_coordinate, uint8_t length, uint8_t open_walls);
void Create_goal(uint8_t x_coordinate, uint8_t y_coordinate,uint8_t length, uint8_t open_walls);
void Update_node(node* p_node, uint8_t length);
node* Exisiting_node_at(uint8_t x_coordinate, uint8_t y_coordinate);
uint8_t Find_index_of_node(node* p_node);
uint8_t Find_low_cost_index();
uint8_t Find_shortest_path(node* p_node1, node* p_node2);
uint8_t Get_cardinal_direction(uint8_t robot_dir, uint8_t turn);
void Do_turn(uint8_t cardinal_direction);
uint8_t Get_dijkpointers_cardinal_direction(node* p_node);
void Follow_path();
node* Easy_find_unexplored_node();
node* Smarter_find_unexplored_node(node* p_node);
void Search(uint8_t sensor_front, uint8_t sensor_front_left, uint8_t sensor_front_right, uint8_t sensor_back_left, uint8_t sensor_back_right);
void Update_map_old(uint8_t sensor_front, uint8_t sensor_front_left, uint8_t sensor_front_right, uint8_t sensor_back_left, uint8_t sensor_back_right);
void Map_init(void);
int Map_main(void);
//---------Nya funktioner börjar här-----------------//
uint8_t Crossing(uint8_t sensor_front, uint8_t sensor_front_left, uint8_t sensor_front_right, uint8_t sensor_back_left, uint8_t sensor_back_right);
void Following_path_at_crossing();
uint8_t Get_new_x_coordinate(uint8_t length);
uint8_t Get_new_y_coordinate(uint8_t length);
uint8_t Number_of_traveled_blocks(uint8_t length);
void Follow(uint8_t sensor_front, uint8_t sensor_front_left, uint8_t sensor_front_right, uint8_t sensor_back_left, uint8_t sensor_back_right);
void Do_level_1(uint8_t sensor_front, uint8_t sensor_front_left, uint8_t sensor_front_right, uint8_t sensor_back_left, uint8_t sensor_back_right);
void Update_map(uint8_t sensor_front, uint8_t sensor_front_left, uint8_t sensor_front_right, uint8_t sensor_back_left, uint8_t sensor_back_right);
void Level_stupid(uint8_t sensor_front, uint8_t sensor_front_left, uint8_t sensor_front_right, uint8_t sensor_back_left, uint8_t sensor_back_right);
void Do_this_when_rfid_found(uint8_t sensor_front_left, uint8_t sensor_front_right, uint8_t sensor_front);
void Set_enable();
#endif
