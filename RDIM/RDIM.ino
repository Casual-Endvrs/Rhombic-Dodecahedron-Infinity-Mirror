#include "RDIM_parameters.h"

#include <random>

#include "momentum_sprite.h"

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
  dueling_unicorns_ani(20); // , leds, nodes_leds
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

void dueling_unicorns_ani(int unicorn_len) // , CRGB leds, int **nodes_leds
{
  clear_all();

  momentum_sprite uni_1(unicorn_len, leds, nodes_leds);
  momentum_sprite uni_2(unicorn_len, leds, nodes_leds);
  // momentum_sprite uni_3(unicorn_len, leds, nodes_leds);

  while (true)
  {
    uni_1.update_position();
    uni_2.update_position();
    // uni_3.update_position();

    render_frame();
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

int find_node_idx(int led_num)
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
  return -1;
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
    start_node_idx = find_node_idx(start_num);

    // find the node where the end led is
    end_led = 1 - 1 * NUM_LEDS_PER_EDGE * (start_led + 1);
    end_node_idx = find_node_idx(end_led); // set end node value

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

//! General functions

float random_probability()
{
  return random(0, __LONG_MAX__) / (__LONG_MAX__ - 1.0);
}

//! Momentum operations
