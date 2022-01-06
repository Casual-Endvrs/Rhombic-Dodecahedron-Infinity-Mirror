#include <WS2812Serial.h>
#define USE_WS2812SERIAL

#include <FastLED.h>
// FASTLED_USING_NAMESPACE

#include <random>

// #define USE_ARDUINO true

#define DATA_PIN 14
#define LED_TYPE WS2811
#define COLOR_ORDER GRB

#define NUM_LEDS_PER_EDGE 18
#define TOTAL_NUM_LEDS 24 * NUM_LEDS_PER_EDGE
#define LED_END_SHIFT 1.2
#define NOT_VALID_LED TOTAL_NUM_LEDS + 1

#define BRIGHTNESS 85
#define FRAMES_PER_SECOND 120

uint8_t gHue = 0; // rotating "base color" used by many of the patterns
uint8_t gHue_ss = 255 / NUM_LEDS_PER_EDGE;

CRGB leds[TOTAL_NUM_LEDS];

// led id number ordered by location around object
// NOT_VALID_LED used as a space filler for non-relevant entries
int nodes_leds[14][4] = {
    {-11, 5, 11, -5},
    {18, 10, -14, NOT_VALID_LED},
    {-21, -19, 21, 19},
    {-6, 20, 2, NOT_VALID_LED},

    {4, -2, -8, NOT_VALID_LED},
    {13, 7, -13, -7},
    {-22, 6, -10, NOT_VALID_LED},

    {-20, 22, 16, NOT_VALID_LED},
    {-3, -17, 3, 17},
    {-12, -18, 14, NOT_VALID_LED},

    {1, -15, -1, 15},
    {12, -16, -24, NOT_VALID_LED},
    {-23, 9, 23, -9},
    {-4, 8, 0, NOT_VALID_LED}};

// vertex coordinates by location around object, first index corresponds to first index of nodes_leds array
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
int led_gears_dir[24] = {
    -1, // {0, 1},
    -1, // {1, 2}, --
    1,  // {2, 3},
    -1, // {3, 4},
    1,  // {4, 5}, --
    1,  // {5, 6},
    -1, // {6, 7}, --
    1,  // {7, 8}, --
    -1, // {8, 9},
    -1, // {9, 10}, --
    -1, // {10, 11}, --
    -1, // {11, 12},
    1,  // {12, 13}, --
    -1, // {13, 14}, --
    1,  // {14, 15},
    1,  // {15, 16}, --
    -1, // {16, 17},
    1,  // {17, 18},
    -1, // {18, 19}, --
    -1, // {19, 20},
    -1, // {20, 21},
    -1, // {21, 22}, --
    1,  // {22, 23}, --
    1,  // {23, 24},
};

// use nodes_leds array and vertex_coords array to determine the location of each led
float led_locs[TOTAL_NUM_LEDS][3];

int height_nodes[14][3][2] = {
    {{-5, 11}, {-11, 5}, {NOT_VALID_LED, NOT_VALID_LED}},
    {{-14, NOT_VALID_LED}, {18, 10}, {NOT_VALID_LED, NOT_VALID_LED}},
    {{19, 21}, {-21, -19}, {NOT_VALID_LED, NOT_VALID_LED}},
    {{-2, NOT_VALID_LED}, {-6, 20}, {NOT_VALID_LED, NOT_VALID_LED}},

    {{-2, NOT_VALID_LED}, {-8, NOT_VALID_LED}, {4, NOT_VALID_LED}},
    {{-13, NOT_VALID_LED}, {-7, 7}, {13, NOT_VALID_LED}},
    {{-10, NOT_VALID_LED}, {6, NOT_VALID_LED}, {-22, NOT_VALID_LED}},

    {{-24, NOT_VALID_LED}, {16, NOT_VALID_LED}, {-20, NOT_VALID_LED}},
    {{3, NOT_VALID_LED}, {17, -17}, {-3, NOT_VALID_LED}},
    {{14, NOT_VALID_LED}, {-18, NOT_VALID_LED}, {-12, NOT_VALID_LED}},

    {{NOT_VALID_LED, NOT_VALID_LED}, {15, -1}, {1, -15}},
    {{NOT_VALID_LED, NOT_VALID_LED}, {22, -16}, {12, NOT_VALID_LED}},
    {{NOT_VALID_LED, NOT_VALID_LED}, {-9, -23}, {23, 9}},
    {{NOT_VALID_LED, NOT_VALID_LED}, {0, 8}, {-4, NOT_VALID_LED}},
};

void print_nodes_leds()
{
  for (int i = 0; i < 14; i++)
  {
    Serial.print("{ ");
    for (int j = 0; j < 4; j++)
    {
      Serial.print(nodes_leds[i][j]);
      Serial.print(", ");
    }
    Serial.println("}");
  }
  Serial.println();
  Serial.println();
}

class unicorn
{
public:
  unicorn(int num_leds);
  void update_position();

  int _pos_leds[2 * NUM_LEDS_PER_EDGE]; // led position of the unicorn

  void print_positions();

private:
  void create_colors();     // method for creating the array used for the hues/colors of the unicorn
  void update_led_colors(); // method that updates the colors of the currently occupied leds set by _pos_leds
  void move();              // method used to advance the unicorns position one place
  int _num_leds;
  CRGB _rgb_colors[2 * NUM_LEDS_PER_EDGE];
  int _dir; // direction the unicorn is travelling
};

void setup()
{
  // set the random seed using noise from pin 0
  randomSeed(analogRead(0));

  Serial.begin(9600);
  Serial.println(NOT_VALID_LED);
  Serial.println(nodes_leds[1][3] == NOT_VALID_LED);
  //Serial.println(gHue_ss);

  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
  delay(3000); // 3 second delay for recovery
  digitalWrite(13, LOW);

  // #if USE_ARDUINO
  //   FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, TOTAL_NUM_LEDS).setCorrection(TypicalLEDStrip);
  // #elif
  //   FastLED.addLeds<6, LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, TOTAL_NUM_LEDS).setCorrection(TypicalLEDStrip);
  // #endif

  LEDS.addLeds<WS2812SERIAL, DATA_PIN, GRB>(leds, TOTAL_NUM_LEDS);

  update_node_nums(nodes_leds);
  update_height_nodes(height_nodes);
  calc_led_locs(led_locs, nodes_leds, vertex_coords);

  print_nodes_leds();

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
}

void loop()
{
  dueling_unicorns_ani(20);
  // twinkling_ani(0.25, 16);

  gear_ani(9, gHue_ss);

  // basic_test();
  // bpm();
  // FastLED.show();
  // FastLED.delay(1000/FRAMES_PER_SECOND);
}

//! Begin - Test Animations

void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8(BeatsPerMinute, 64, 255);
  for (int i = 0; i < TOTAL_NUM_LEDS; i++)
  { //9948
    leds[i] = ColorFromPalette(palette, gHue + (i * 2), beat - gHue + (i * 10));
  }
}

void fadeall()
{
  for (int i = 0; i < TOTAL_NUM_LEDS; i++)
  {
    leds[i].nscale8(250);
  }
}

void basic_test()
{
  static uint8_t hue = 0;
  // First slide the led in one direction
  for (int i = 0; i < TOTAL_NUM_LEDS; i++)
  {
    // Set the i'th led to red
    leds[i] = CHSV(hue++, 255, 255);
    // Show the leds
    render_frame();
    // now that we've shown the leds, reset the i'th led to black
    // leds[i] = CRGB::Black;
    fadeall();
  }
}
//! End - Test Animations

//! Begin - Animations
void gear_ani(int speed, uint8_t deltahue)
{
  CHSV hsv;
  hsv.hue = 0;
  hsv.val = 255;
  hsv.sat = 240;

  uint8_t base_hue = 0;

  int led_num;
  int led_pos;
  int strip_num;
  int dir;

  while (true)
  {
    hsv.hue = base_hue;
    for (led_pos = 0; led_pos < NUM_LEDS_PER_EDGE; led_pos++)
    {
      hsv.hue = hsv.hue + deltahue;
      for (strip_num = 0; strip_num < 24; strip_num++)
      {
        dir = led_gears_dir[strip_num];
        if (dir < 0)
        {
          led_num = (strip_num + 1) * NUM_LEDS_PER_EDGE - led_pos - 1;
        }
        else
        {
          led_num = strip_num * NUM_LEDS_PER_EDGE + led_pos;
        }
        leds[led_num] = hsv;
      }
    }
    render_frame();
    base_hue = base_hue + speed;
  }
}

void twinkling_ani(float prob_new_light, u_int8_t fade_rate)
{
  clear_all();
  int random_led;
  float prob_create_new;

  while (true)
  {
    prob_create_new = random_probability();
    if (prob_create_new < prob_new_light)
    {
      random_led = random(0, TOTAL_NUM_LEDS + 1);
      leds[random_led] = CRGB::White;
    }
    fadeToBlackBy(leds, TOTAL_NUM_LEDS, fade_rate);
    render_frame();
  }
}

void dueling_unicorns_ani(int unicorn_len)
{
  clear_all();

  // unicorn uni_1(unicorn_len);
  unicorn uni_2(unicorn_len);
  // unicorn uni_3(unicorn_len);

  while (true)
  {
    // uni_1.update_position();
    uni_2.update_position();
    // uni_3.update_position();

    render_frame();

    // if (uni_1._pos_leds[0] >= TOTAL_NUM_LEDS)
    // {
    //   delay(1000000);
    // }
  }
}

void render_frame()
{
  FastLED.show();
  // if (limit_fps)
  FastLED.delay(1000 / FRAMES_PER_SECOND);
}

void clear_all()
{
  fadeToBlackBy(leds, TOTAL_NUM_LEDS, 255);
  render_frame();
}

//! End - Animations

int find_node_idx(int led_num, int nodes_leds[14][4])
{
  for (int node_num = 0; node_num < 14; node_num++)
  { // iterate through nodes
    for (int led_idx = 0; led_idx < 4; led_idx++)
    { // iterate through leds in node
      if (led_num == nodes_leds[node_num][led_idx])
      {                  // if the correct node has been found
        return node_num; // return the node index value
      }
    }
  }
}

void calc_led_locs(float led_locs[TOTAL_NUM_LEDS][3], int nodes_leds[14][4], float vertex_coords[14][3])
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
    start_node_idx = find_node_idx(start_num, nodes_leds);

    // find the node where the end led is
    end_led = 1 - 1 * NUM_LEDS_PER_EDGE * (start_led + 1);
    end_node_idx = find_node_idx(end_led, nodes_leds); // set end node value

    // calculate values required to determine the location of individual leds
    float start_point[3];
    float deltas[3]; // length from start to finish of led string
    for (int i = 0; i < 3; i++)
    {                                                                                // calculate length for each dimensions
      start_point[i] = vertex_coords[start_node_idx][i];                             // start point of led string
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

void update_node_nums(int nodes_leds[14][4])
{
  int led_num;
  for (int node = 0; node < 14; node++)
  {
    for (int led = 0; led < 4; led++)
    {
      led_num = nodes_leds[node][led];
      nodes_leds[node][led] = calc_led_num(led_num);
    }
  }
}

/**
 * @brief This function iteratively works to convert all of the led ids to the 
 * corresponding led number according to the leds location along the led string.
 * 
 * @param height_nodes The 3D array that contains all of the required led ids 
 * based on their location across the height of the RDIM.
 */
void update_height_nodes(int height_nodes[14][3][2])
{
  int led_id;
  for (int node = 0; node < 14; node++)
  { // for each node
    for (int layer = 0; layer < 3; layer++)
    { // for each layer: bottom, middle, top
      for (int side = 0; side < 2; side++)
      { // for each led on the side of each layer
        led_id = height_nodes[node][layer][side];
        // set the led number
        height_nodes[node][layer][side] = calc_led_num(led_id);
      }
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

//! color functions/classes ?

//! Unicorn class functions

unicorn::unicorn(int num_leds)
{
  if (num_leds > 2 * NUM_LEDS_PER_EDGE)
  {
    num_leds = 2 * NUM_LEDS_PER_EDGE;
  }
  _num_leds = num_leds;
  create_colors();

  // create initial head position and direction
  _pos_leds[0] = random(TOTAL_NUM_LEDS - 1);
  _dir = 2 * random(2) - 1;

  // move num_leds times to obtain complete unicorn position
  for (int i = 0; i < num_leds - 1; i++)
  {
    move();
  }
}

void unicorn::update_position()
{ // set last led black
  leds[_pos_leds[_num_leds - 1]] = CRGB::Black;

  // move to the next position
  move();

  // update new colors
  update_led_colors();
}

void unicorn::create_colors()
{
  CHSV hsv(0, 255, 255);
  u_int8_t _hue_delta = 255 / (_num_leds - 1); // hue delta between two neighbouring leds

  for (int i = 0; i < _num_leds; i++)
  {
    hsv.hue = hsv.hue + _hue_delta;
    hsv2rgb_rainbow(hsv, _rgb_colors[i]);
  }
}

void unicorn::update_led_colors()
{
  for (int i = _num_leds; i >= 0; i--)
  {
    leds[_pos_leds[i]] = _rgb_colors[i];
  }
}

void unicorn::move()
{
  int indices[2];
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
    if (nodes_leds[indices[0]][3] == NOT_VALID_LED)
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
    new_head = nodes_leds[indices[0]][indices[1]];
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

void unicorn::print_positions()
{
  Serial.print("\n\n{");
  for (int i = 0; i < _num_leds; i++)
  {
    Serial.print(_pos_leds[i]);
    Serial.print(", ");
  }
  Serial.print("}\n\n");
}

//! General functions

void shift_arr_down(int *arr, int arr_len)
{
  for (int i = arr_len - 1; i > 0; i--)
  {
    arr[i] = arr[i - 1];
  }
}

float random_probability()
{
  return random(0, __LONG_MAX__) / (__LONG_MAX__ - 1.0);
}

//! Momentum operations

bool test_at_node(int led_num, int direction)
{
  if ((led_num + (direction + 1) / 2) == 0)
  {
    return true;
  }
  return false;
}

bool test_is_4_node(int *indices)
{
  if (nodes_leds[indices[0]][3] == NOT_VALID_LED)
  {
    return false;
  }
  return true;
}

void go_random_lrs(int *indicies, float prob_go_straight) // go a random direction, left / right / straight
{
  float turn_threshold = (1 - prob_go_straight) / 2.;
  float turn_prob;
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

void find_momentum_node(int led_num, int *indices) // find the index of the momentum node and entry of the given led
{
  bool stop = false;
  for (int node_idx = 0; node_idx < 14; node_idx++)
  {
    for (int entry_idx = 0; entry_idx < 4; entry_idx++)
    {
      if (nodes_leds[node_idx][entry_idx] == led_num)
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

void turn_right(int *indices)
{
  indices[1] = (indices[1] + 3) % 4; // +3 has the same effect as -1
  if (nodes_leds[indices[0]][indices[1]] == NOT_VALID_LED)
  {
    indices[1] = (indices[1] + 3) % 4;
  }
}

void turn_left(int *indices)
{
  indices[1] = (indices[1] + 1) % 4;
  if (nodes_leds[indices[0]][indices[1]] == NOT_VALID_LED)
  {
    indices[1] = (indices[1] + 1) % 4;
  }
}

void go_straight(int *indices)
{
  // if straight is not an option, the same value is returned
  // this acts like a reflection
  indices[1] = (indices[1] + 2) % 4;
  if (nodes_leds[indices[0]][indices[1]] == NOT_VALID_LED)
  {
    indices[1] = (indices[1] + 2) % 4;
  }
}
