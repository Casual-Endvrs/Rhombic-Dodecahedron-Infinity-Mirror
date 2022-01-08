#include "Arduino.h"
#include "momentum_sprite.h"
#include <FastLED.h>

momentum_sprite::momentum_sprite(int num_leds, CRGB *leds, int nodes_leds[14][4]) // , CRGB leds, int *nodes_leds
{
    if (num_leds > 2 * NUM_LEDS_PER_EDGE)
    {
        num_leds = 2 * NUM_LEDS_PER_EDGE;
    }

    _num_leds = num_leds;
    create_colors();

    _nodes_leds = nodes_leds + 1;
    _leds = leds;

    // create initial head position and direction
    _pos_leds[0] = random(TOTAL_NUM_LEDS - 1);
    _dir = 2 * random(2) - 1;

    // move num_leds times to obtain complete momentum_sprite position
    for (int i = 0; i < num_leds - 1; i++)
    {
        move();
    }
}

void momentum_sprite::update_position()
{
    // move to the next position
    move();

    // update new colors
    update_led_colors();
}

void momentum_sprite::create_colors()
{
    CHSV hsv(0, 255, 255);
    u_int8_t _hue_delta = 255 / (_num_leds - 2); // hue delta between two neighbouring leds

    for (int i = 0; i < _num_leds - 1; i++)
    {
        hsv.hue = hsv.hue + _hue_delta;
        hsv2rgb_rainbow(hsv, _rgb_colors[i]);
    }
    _rgb_colors[_num_leds - 1] = CRGB::Black;
}

void momentum_sprite::update_led_colors()
{
    for (int i = _num_leds; i >= 0; i--)
    {
        _leds[_pos_leds[i]] = _rgb_colors[i];
    }
}

void momentum_sprite::move()
{
    int indices[2] = {0, 0};
    bool at_strip_end = false;
    int new_head;

    if (_dir == -1) // travelling toward the beginning of the led strip
    {
        if ((_pos_leds[0] % NUM_LEDS_PER_EDGE) == 0) // at the start of led strip
        {
            at_strip_end = true;
            find_momentum_node(_pos_leds[0], indices);
        }
    }
    else // travelling toward the end of the led strip
    {
        if ((_pos_leds[0] + 1) % NUM_LEDS_PER_EDGE == 0) // at the start of led strip
        {
            at_strip_end = true;
            find_momentum_node(-1 * _pos_leds[0], indices);
        }
    }

    if (at_strip_end)
    {
        // special action required to turn some direction
        int turn_dir;

        // if node contains 3 strips - straight is NOT an option
        if (_nodes_leds[indices[0]][3] == NOT_VALID_LED)
        {
            turn_dir = random(2);
        }
        else // if node contains 4 strips - straight is an option
        {
            turn_dir = random(3);
        }

        // find new head led
        if (turn_dir == 0) // turn left
        {
            turn_left(indices);
        }
        else if (turn_dir == 1) // turn right
        {
            turn_right(indices);
        }
        else // go straight
        {
            go_straight(indices);
        }
        new_head = _nodes_leds[indices[0]][indices[1]];
    }
    else
    {
        new_head = _pos_leds[0] + _dir;
    }

    // shift led position numbers down
    shift_arr_down(_pos_leds, _num_leds);

    // insert new head led num
    _pos_leds[0] = new_head;

    if (at_strip_end)
    {
        if (_pos_leds[0] < 0)
        {
            _dir = -1;
            _pos_leds[0] = -1 * _pos_leds[0];
        }
        else
        {
            _dir = 1;
        }
    }
}

void momentum_sprite::print_positions()
{
    Serial.print("\n\n{");
    for (int i = 0; i < _num_leds; i++)
    {
        Serial.print(_pos_leds[i]);
        Serial.print(", ");
    }
    Serial.print("}\n\n");
}

bool momentum_sprite::test_at_node(int led_num, int direction)
{
    if ((led_num + (direction + 1) / 2) == 0)
    {
        return true;
    }
    return false;
}

bool momentum_sprite::test_is_4_node(int *indices)
{
    if (_nodes_leds[indices[0]][3] == NOT_VALID_LED)
    {
        return false;
    }
    return true;
}

void momentum_sprite::go_random_lrs(int *indicies, float prob_go_straight) // go a random direction, left / right / straight
{
    float turn_threshold = (1 - prob_go_straight) / 2.;
    float turn_prob = random(0, __LONG_MAX__) / (__LONG_MAX__ - 1.0);
    if (turn_prob < turn_threshold)
    {
        turn_left(indicies);
    }
    else if (turn_prob >= (1 - turn_threshold))
    {
        turn_right(indicies);
    }
    else
    {
        go_straight(indicies);
    }
}

void momentum_sprite::find_momentum_node(int led_num, int *indices) // find the index of the momentum node and entry of the given led
{
    bool stop = false;
    for (int node_idx = 0; node_idx < 14; node_idx++)
    {
        for (int entry_idx = 0; entry_idx < 4; entry_idx++)
        {
            if (_nodes_leds[node_idx][entry_idx] == led_num)
            {
                indices[0] = node_idx;
                indices[1] = entry_idx;
                stop = true;
            }
            if (stop)
            {
                break;
            }
        }
        if (stop)
        {
            break;
        }
    }
}

void momentum_sprite::turn_right(int *indices)
{
    indices[1] = (indices[1] + 3) % 4; // +3 has the same effect as -1
    if (_nodes_leds[indices[0]][indices[1]] == NOT_VALID_LED)
    {
        indices[1] = (indices[1] + 3) % 4;
    }
}

void momentum_sprite::turn_left(int *indices)
{
    indices[1] = (indices[1] + 1) % 4;
    if (_nodes_leds[indices[0]][indices[1]] == NOT_VALID_LED)
    {
        indices[1] = (indices[1] + 1) % 4;
    }
}

void momentum_sprite::go_straight(int *indices)
{
    // if straight is not an option, the same value is returned
    // this acts like a reflection
    indices[1] = (indices[1] + 2) % 4;
    if (_nodes_leds[indices[0]][indices[1]] == NOT_VALID_LED)
    {
        indices[1] = (indices[1] + 2) % 4;
    }
}

void momentum_sprite::shift_arr_down(int *arr, int arr_len)
{
    for (int i = arr_len - 1; i > 0; i--)
    {
        arr[i] = arr[i - 1];
    }
}

// // int momentum_sprite::find_node_idx(int led_num)
// // {
// //     for (int node_num = 0; node_num < 14; node_num++)
// //     { // iterate through nodes
// //         for (int led_idx = 0; led_idx < 4; led_idx++)
// //         { // iterate through leds in node
// //             if (led_num == _nodes_leds[node_num*14+led_idx])
// //             {                    // if the correct node has been found
// //                 return node_num; // return the node index value
// //             }
// //         }
// //     }
// // }