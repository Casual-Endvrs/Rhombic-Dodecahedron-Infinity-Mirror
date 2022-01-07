/*
    momentum_sprite.h - Library for creating and running momentum sprites for the Rhombic Dodecahedron Infinity Mirror
    Created by Braden Wiens, January 6, 2022
*/

#ifndef momentum_sprite_h
#define momentum_sprite_h

#include "Arduino.h"
#include "RDIM_parameters.h"

class momentum_sprite
{
public:
    momentum_sprite(int num_leds, CRGB *leds, int nodes_leds[14][4]); // , CRGB leds, int *nodes_leds
    void update_position();

    void print_positions(); //! diagnostic and needs to be removed

private:
    void create_colors();     // method for creating the array used for the hues/colors of the momentum_sprite
    void update_led_colors(); // method that updates the colors of the currently occupied leds set by _pos_leds
    void move();              // method used to advance the momentum_sprite position one place
    bool test_at_node(int led_num, int direction);
    bool test_is_4_node(int *indices);
    void go_random_lrs(int *indicies, float prob_go_straight);
    void find_momentum_node(int led_num, int *indices);
    void turn_right(int *indices);
    void turn_left(int *indices);
    void go_straight(int *indices);
    void shift_arr_down(int *arr, int arr_len);
    int find_node_idx(int led_num, int nodes_leds[14][4]);

    int _num_leds;
    int (*_nodes_leds)[4];
    CRGB *_leds;
    CRGB _rgb_colors[2 * NUM_LEDS_PER_EDGE];
    int _pos_leds[2 * NUM_LEDS_PER_EDGE]; // led position of the momentum_sprite
    int _dir;                             // direction the momentum_sprite is travelling
};

#endif
