#include <iostream>

#define NUM_LEDS_PER_EDGE 3
#define TOTAL_NUM_LEDS 24 * NUM_LEDS_PER_EDGE
#define LED_END_SHIFT 0
#define NOT_VALID_LED TOTAL_NUM_LEDS + 1

using namespace std;

// ! diagnostic functions
void cout_arr_2d(int arr[2])
{
	cout << "[" << arr[0] << ", ";
	// cout << arr[1] << ", ";
	cout << arr[1] << "]" << endl;
}

void cout_arr_3d(float arr[3])
{
	cout << "[" << arr[0] << ", ";
	cout << arr[1] << ", ";
	cout << arr[2] << "]" << endl;
}

void cout_arr_4d(int arr[4])
{
	cout << "[" << arr[0] << ", ";
	cout << arr[1] << ", ";
	cout << arr[2] << ", ";
	cout << arr[3] << "]" << endl;
}

// ! begin - potentially unnecessary arrays
// The simple node branches can be used to navigator branches
// Nodes with 3 branches
int n3[8][3] = {
	{18, 10, -14},
	{6, 20, 2},
	{4, -2, -8},
	{-22, 6, -10},
	{-20, -24, 16},
	{-12, -18, 14},
	{12, -16, 22},
	{-4, 8, 0},
};

// Nodes with 4 branches
int n4[6][4] = {
	{-21, -19, 21, 19},
	{-11, 5, 11, -5},
	{13, 7, -13, -7},
	{-3, -17, 3, 17},
	{1, -15, -1, 15},
	{23, 9, -23, -9},
};
// ! end - potentially unnecessary arrays

// led id number ordered by location around object
// NOT_VALID_LED used as a space filler for non-relevant entries
int led_nums[14][4] = {
	{-11, 5, 11, -5},
	{18, 10, -14, NOT_VALID_LED},
	{-21, -19, 21, 19},
	{-6, 20, 2, NOT_VALID_LED},

	{4, -2, -8, NOT_VALID_LED},
	{13, 7, -13, -7},
	{-22, 6, -10, NOT_VALID_LED},

	{-20, -24, 16, NOT_VALID_LED},
	{-3, -17, 3, 17},
	{-12, -18, 14, NOT_VALID_LED},

	{1, -15, -1, 15},
	{12, -16, 22, NOT_VALID_LED},
	{23, 9, -23, -9},
	{-4, 8, 0, NOT_VALID_LED}};

// vertex coordinates by location around object, first index corresponds to first index of led_nums array
float vertex_coords[14][3] = {
	{1, 0, 1},
	{0, -1, 1},
	{-1, 0, 1},
	{0, 1, 1},

	{1, -1, 0},
	{0, -2, 0},
	{-1, -1, 0},

	{-1, 1, 0},
	{0, 2, 0},
	{1, 1, 0},

	{1, 0, -1},
	{0, -1, -1},
	{-1, 0, -1},
	{0, 1, -1}};

// Array containing the direction the color scheme needs to travel to make gears work
int led_ids_gears[24][2] = {
	{1 * NUM_LEDS_PER_EDGE, 0 * NUM_LEDS_PER_EDGE},
	{1 * NUM_LEDS_PER_EDGE, 2 * NUM_LEDS_PER_EDGE},
	{2 * NUM_LEDS_PER_EDGE, 3 * NUM_LEDS_PER_EDGE},
	{4 * NUM_LEDS_PER_EDGE, 3 * NUM_LEDS_PER_EDGE},
	{5 * NUM_LEDS_PER_EDGE, 4 * NUM_LEDS_PER_EDGE},
	{5 * NUM_LEDS_PER_EDGE, 6 * NUM_LEDS_PER_EDGE},
	{6 * NUM_LEDS_PER_EDGE, 7 * NUM_LEDS_PER_EDGE},
	{8 * NUM_LEDS_PER_EDGE, 7 * NUM_LEDS_PER_EDGE},
	{9 * NUM_LEDS_PER_EDGE, 8 * NUM_LEDS_PER_EDGE},
	{9 * NUM_LEDS_PER_EDGE, 10 * NUM_LEDS_PER_EDGE},
	{10 * NUM_LEDS_PER_EDGE, 11 * NUM_LEDS_PER_EDGE},
	{12 * NUM_LEDS_PER_EDGE, 11 * NUM_LEDS_PER_EDGE},
	{13 * NUM_LEDS_PER_EDGE, 12 * NUM_LEDS_PER_EDGE},
	{13 * NUM_LEDS_PER_EDGE, 14 * NUM_LEDS_PER_EDGE},
	{14 * NUM_LEDS_PER_EDGE, 15 * NUM_LEDS_PER_EDGE},
	{16 * NUM_LEDS_PER_EDGE, 15 * NUM_LEDS_PER_EDGE},
	{17 * NUM_LEDS_PER_EDGE, 16 * NUM_LEDS_PER_EDGE},
	{17 * NUM_LEDS_PER_EDGE, 18 * NUM_LEDS_PER_EDGE},
	{18 * NUM_LEDS_PER_EDGE, 19 * NUM_LEDS_PER_EDGE},
	{20 * NUM_LEDS_PER_EDGE, 19 * NUM_LEDS_PER_EDGE},
	{21 * NUM_LEDS_PER_EDGE, 20 * NUM_LEDS_PER_EDGE},
	{21 * NUM_LEDS_PER_EDGE, 22 * NUM_LEDS_PER_EDGE},
	{22 * NUM_LEDS_PER_EDGE, 23 * NUM_LEDS_PER_EDGE},
	{24 * NUM_LEDS_PER_EDGE, 23 * NUM_LEDS_PER_EDGE},
};

// Array containing the direction the color scheme needs to travel to make gears work
int led_gears_dir[24] = {-1, 1, 1, -1, -1, 1, 1, -1, -1, 1, 1, -1, -1, 1, 1, -1, -1, 1, 1, -1, -1, 1, 1, -1};

// use led_nums array and vertex_coords array to determine the location of each led
float led_locs[TOTAL_NUM_LEDS][3];

int height_nodes[14][3][2] = {
	{{-5 * NUM_LEDS_PER_EDGE + 1, 11 * NUM_LEDS_PER_EDGE}, {-11 * NUM_LEDS_PER_EDGE + 1, 5 * NUM_LEDS_PER_EDGE}, {NOT_VALID_LED, NOT_VALID_LED}},
	{{-14 * NUM_LEDS_PER_EDGE + 1, NOT_VALID_LED}, {18 * NUM_LEDS_PER_EDGE, 10 * NUM_LEDS_PER_EDGE}, {NOT_VALID_LED, NOT_VALID_LED}},
	{{19 * NUM_LEDS_PER_EDGE, 21 * NUM_LEDS_PER_EDGE}, {-21 * NUM_LEDS_PER_EDGE + 1, -19 * NUM_LEDS_PER_EDGE + 1}, {NOT_VALID_LED, NOT_VALID_LED}},
	{{-2 * NUM_LEDS_PER_EDGE + 1, NOT_VALID_LED}, {-6 * NUM_LEDS_PER_EDGE + 1, 20 * NUM_LEDS_PER_EDGE}, {NOT_VALID_LED, NOT_VALID_LED}},

	{{-2 * NUM_LEDS_PER_EDGE + 1, NOT_VALID_LED}, {-8 * NUM_LEDS_PER_EDGE + 1, NOT_VALID_LED}, {4 * NUM_LEDS_PER_EDGE, NOT_VALID_LED}},
	{{-13 * NUM_LEDS_PER_EDGE + 1, NOT_VALID_LED}, {-7 * NUM_LEDS_PER_EDGE + 1, 7 * NUM_LEDS_PER_EDGE}, {13 * NUM_LEDS_PER_EDGE, NOT_VALID_LED}},
	{{-10 * NUM_LEDS_PER_EDGE + 1, NOT_VALID_LED}, {6 * NUM_LEDS_PER_EDGE, NOT_VALID_LED}, {-22 * NUM_LEDS_PER_EDGE + 1, NOT_VALID_LED}},

	{{-24 * NUM_LEDS_PER_EDGE + 1, NOT_VALID_LED}, {16 * NUM_LEDS_PER_EDGE, NOT_VALID_LED}, {-20 * NUM_LEDS_PER_EDGE + 1, NOT_VALID_LED}},
	{{3 * NUM_LEDS_PER_EDGE, NOT_VALID_LED}, {17 * NUM_LEDS_PER_EDGE, -17 * NUM_LEDS_PER_EDGE + 1}, {-3 * NUM_LEDS_PER_EDGE + 1, NOT_VALID_LED}},
	{{14 * NUM_LEDS_PER_EDGE, NOT_VALID_LED}, {-18 * NUM_LEDS_PER_EDGE + 1, NOT_VALID_LED}, {-12 * NUM_LEDS_PER_EDGE + 1, NOT_VALID_LED}},

	{{NOT_VALID_LED, NOT_VALID_LED}, {15 * NUM_LEDS_PER_EDGE, -1 * NUM_LEDS_PER_EDGE + 1}, {1 * NUM_LEDS_PER_EDGE, -15 * NUM_LEDS_PER_EDGE + 1}},
	{{NOT_VALID_LED, NOT_VALID_LED}, {22 * NUM_LEDS_PER_EDGE, -16 * NUM_LEDS_PER_EDGE + 1}, {12 * NUM_LEDS_PER_EDGE, NOT_VALID_LED}},
	{{NOT_VALID_LED, NOT_VALID_LED}, {-9 * NUM_LEDS_PER_EDGE + 1, -23 * NUM_LEDS_PER_EDGE + 1}, {23 * NUM_LEDS_PER_EDGE, 9 * NUM_LEDS_PER_EDGE}},
	{{NOT_VALID_LED, NOT_VALID_LED}, {0 * NUM_LEDS_PER_EDGE, 8 * NUM_LEDS_PER_EDGE}, {-4 * NUM_LEDS_PER_EDGE + 1, NOT_VALID_LED}},
};

int find_node_idx(int led_num, int led_nums[14][4])
{
	for (int node_num = 0; node_num < 14; node_num++)
	{ // iterate through nodes
		for (int led_idx = 0; led_idx < 4; led_idx++)
		{ // iterate through leds in node
			if (led_num == led_nums[node_num][led_idx])
			{					 // if the correct node has been found
				return node_num; // return the node index value
			}
		}
	}
}

void calc_led_locs(float led_locs[TOTAL_NUM_LEDS][3], int led_nums[14][4], float vertex_coords[14][3])
{
	int led_num = 0;
	int start_num;
	int end_led;
	int start_node_idx;
	int end_node_idx;
	for (int start_led = 0; start_led < 24; start_led++)
	{ // for each possible starting led id
		// find the node where the start led is
		start_num = start_led * NUM_LEDS_PER_EDGE;
		start_node_idx = find_node_idx(start_num, led_nums);

		// find the node where the end led is
		end_led = -1 * (start_led + 1);
		end_node_idx = find_node_idx(end_led, led_nums); // set end node value

		// calculate values required to determine the location of individual leds
		float start_point[3];
		float deltas[3]; // length from start to finish of led string
		for (int i = 0; i < 3; i++)
		{																				   // calculate length for each dimensions
			start_point[i] = vertex_coords[start_node_idx][i];							   // start point of led string
			deltas[i] = vertex_coords[end_node_idx][i] - vertex_coords[start_node_idx][i]; // calculate spacing between each led
			deltas[i] = deltas[i] / (2 * LED_END_SHIFT + NUM_LEDS_PER_EDGE - 1);
		}

		for (int edge_led_num = 0; edge_led_num < NUM_LEDS_PER_EDGE; edge_led_num++)
		{ // for each led along a given edge
			for (int i = 0; i < 3; i++)
			{ // for x,y,z coordinate of the selected led, set its location
				led_locs[led_num][i] = start_point[i] + deltas[i] * (LED_END_SHIFT + edge_led_num);
			}
			led_num++; // advance to the next led
		}
	}
}

/**
 * @brief This function is used to convert raw led ids to the actual number that 
 * corresponds to the leds location along the led string.
 * 
 * @param led_id The raw led id.
 * @return int The led number.
 */
int calc_led_num(int led_id)
{
	if (led_id == NOT_VALID_LED)
	{ // if it is not an led, return not an led
		return NOT_VALID_LED;
	}
	if (led_id < 0)
	{ // if the led is the end of the string
		return led_id * NUM_LEDS_PER_EDGE + 1;
	}
	else
	{ // if the led is the start of the sting
		return led_id * NUM_LEDS_PER_EDGE;
	}
}

void update_node_nums(int led_nums[14][4])
{
	int led_num;
	for (int node = 0; node < 14; node++)
	{
		for (int led = 0; led < 4; led++)
		{
			led_num = led_nums[node][led];
			led_nums[node][led] = calc_led_num(led_num);
		}
	}
}

int start_led_num(int led_id)
{
	return led_id * NUM_LEDS_PER_EDGE;
}

int end_led_num(int led_id)
{
	return led_id * NUM_LEDS_PER_EDGE - 1;
}

int main()
{
	update_node_nums(led_nums);
	calc_led_locs(led_locs, led_nums, vertex_coords);

	for (int i = 0; i < 14; i++)
	{
		break;
		cout_arr_4d(led_nums[i]);
	}

	for (int i = 0; i < TOTAL_NUM_LEDS; i++)
	{
		break;
		if (i % NUM_LEDS_PER_EDGE == 0)
		{
			cout << endl;
		}
		cout_arr_3d(led_locs[i]);
	}
}
