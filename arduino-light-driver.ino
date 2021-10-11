
/*******************************************************************************
* Includes
*******************************************************************************/
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

/*******************************************************************************
* Defines
*******************************************************************************/
#define DATA_PIN  6
#define NUMPIXELS 48
#define LED_PIN 13
#define DELAYVAL 500

/*******************************************************************************
* Local Types and Typedefs
*******************************************************************************/

/*******************************************************************************
* Global Variables
*******************************************************************************/
Adafruit_NeoPixel pixels(NUMPIXELS, DATA_PIN, NEO_GRB + NEO_KHZ800);

/*******************************************************************************
* Static Function Prototypes
*******************************************************************************/
static void initialise_gpio(void);
static void initialise_lights(void);
static int get_light_mode(void);
static void handle_light_mode(int light_mode);

/*******************************************************************************
* Static Variables
*******************************************************************************/

/*******************************************************************************
* Global Functions
*******************************************************************************/

void setup() {
  initialise_gpio();
  initialise_lights();
}

void loop() {
  int light_mode = get_light_mode();
  handle_light_mode(light_mode);
}

/*******************************************************************************
* Static Functions
*******************************************************************************/

static void initialise_gpio(void) {
  pinMode(LED_PIN, OUTPUT);
}

static void initialise_lights(void) {
  pixels.begin();
}

static int get_light_mode(void) {
  return 0;
}

static void handle_light_mode(int light_mode) {
  static int cycle_counter = 0;
  cycle_counter += 1;

  switch(light_mode) {
    case 0:  { rotating_rainbow(cycle_counter);   }break;
    case 1:  { rotating_colour(cycle_counter, 0);       }break;
    case 2:  { rotating_colour(cycle_counter, 33);     }break;
    case 3:  { rotating_colour(cycle_counter, 66);      }break;
    case 4:  { colour_cycling(cycle_counter);     }break;
    default: { lights_off();                      }break;
  }
}

static void colour_cycling(int cycle_counter) {
  int brightness_level = 100;
  int rainbow_position = cycle_counter % 100;
  int red_term = get_red_component(rainbow_position, brightness_level);
  int green_term = get_green_component(rainbow_position, brightness_level);
  int blue_term = get_blue_component(rainbow_position, brightness_level);
    
  pixels.clear();
  for(int i = 0; i < NUMPIXELS; i++) 
  {
    pixels.setPixelColor(i, pixels.Color(red_term, green_term, blue_term));
  }
  pixels.show();
}

static void rotating_colour(int cycle_counter, int rainbow_position) {
  pixels.clear();
  for(int i = 0; i < NUMPIXELS; i++) 
  {
    int offset = (i + cycle_counter) % NUMPIXELS;
    int brightness_level = (100 * offset) / NUMPIXELS;
    int red_term = get_red_component(rainbow_position, brightness_level);
    int green_term = get_green_component(rainbow_position, brightness_level);
    int blue_term = get_blue_component(rainbow_position, brightness_level);
    pixels.setPixelColor(i, pixels.Color(red_term, green_term, blue_term));
  }
  pixels.show();
}

static void rotating_rainbow(int cycle_counter) {
  pixels.clear();
  for(int i=0; i<NUMPIXELS; i++) 
  {
    int brightness_level = 100;
    int rainbow_position = (i + cycle_counter) % 100;
    int red_term = get_red_component(rainbow_position, brightness_level);
    int green_term = get_green_component(rainbow_position, brightness_level);
    int blue_term = get_blue_component(rainbow_position, brightness_level);
    pixels.setPixelColor(i, pixels.Color(red_term, green_term, blue_term));
  }
  pixels.show();
}

static void lights_off() {
  pixels.clear();
  pixels.show();
}

static int get_red_component(int rainbow_position, int brightness_level)
{
  return get_colour_component(rainbow_position - 0, brightness_level);
}

static int get_green_component(int rainbow_position, int brightness_level)
{
  return get_colour_component(rainbow_position - 33, brightness_level);
}

static int get_blue_component(int rainbow_position, int brightness_level)
{
  return get_colour_component(rainbow_position - 66, brightness_level);
}

static int get_colour_component(int rainbow_position, int brightness_level) {
  const int max_brightness = 100;
  const int colour_extent = 33;
  const int half_colour_extent = colour_extent >> 1;
  const int max_colour_tem = 255;
  int colour_term = 0;

  /* Establish the colour weight */
  int colour_offset = abs(rainbow_position);
  if (colour_offset > colour_extent) 
  {
    colour_term = 0;
  }
  else if (colour_offset < half_colour_extent) 
  {
    colour_term = max_colour_tem;
  }
  else 
  {
    int dimming_range_offset = colour_offset - half_colour_extent;
    colour_term = max_colour_tem - ((max_colour_tem  * dimming_range_offset) / half_colour_extent);
  }

  /* Scale it for the brightness level */
  if (brightness_level > max_brightness) 
  {
    brightness_level = max_brightness;
  }
  if (brightness_level < 0) 
  {
    brightness_level = 0;
  }
  colour_term = (colour_term * brightness_level) / max_brightness;
  return colour_term;
}
