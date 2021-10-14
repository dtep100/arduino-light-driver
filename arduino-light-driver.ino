
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
#define DELAYVAL 50
#define GAMMA_VALUE 192

/*******************************************************************************
* Local Types and Typedefs
*******************************************************************************/

/*******************************************************************************
* Global Variables
*******************************************************************************/
Adafruit_NeoPixel pixels(NUMPIXELS, DATA_PIN, NEO_GRB + NEO_KHZ800);

/*******************************************************************************
* Static Variables
*******************************************************************************/

/*******************************************************************************
* Main Functions
*******************************************************************************/

void setup() {
  initialise_gpio();
  initialise_lights();
}

void loop() {
  int light_mode = get_light_mode();
  handle_light_mode(light_mode);

  digitalWrite(LED_PIN, HIGH);
  delay(DELAYVAL);
  digitalWrite(LED_PIN, LOW);
  delay(DELAYVAL);
}

static void initialise_gpio(void) {
  pinMode(LED_PIN, OUTPUT);
}

static void initialise_lights(void) {
  pixels.begin();
}

static int get_light_mode(void) {
  static int cycle_counter = 0;
  cycle_counter += 1;
  int light_mode = (cycle_counter >> 8) & 0x7;
  return light_mode;
}

static void handle_light_mode(int light_mode) {
  static int cycle_counter = 0;
  cycle_counter += 1;

  switch(light_mode) {
    case 0:  { rotating_rainbow(cycle_counter);     }break;
    case 1:  { rotating_colour(cycle_counter, 0);   }break;
    case 2:  { rotating_colour(cycle_counter, 33);  }break;
    case 3:  { rotating_colour(cycle_counter, 66);  }break;
    case 4:  { colour_cycling(cycle_counter);       }break;
    case 5:  { fading_colour(cycle_counter, 75);    }break;
    case 6:  { rotating_rainbow(cycle_counter);     }break;
    default: { lights_off();                        }break;
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

static void fading_colour(int cycle_counter, int rainbow_position) {
  int brightness_level = abs((cycle_counter % (100 << 1)) - 100);
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
  int peak_idx = cycle_counter % NUMPIXELS;
  for(int i = 0; i < NUMPIXELS; i++) 
  {
    int distance = get_distance_between(peak_idx, i);
    int brightness_level = ((((NUMPIXELS >> 1) - distance) * 100) / (NUMPIXELS >> 1));
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
    int rainbow_position = (((i + cycle_counter) % NUMPIXELS) * 100) / NUMPIXELS;
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


/*******************************************************************************
* Utility Functions
*******************************************************************************/

static void send_debug(int index, int red_term, int green_term, int blue_term)
{
  static bool initialised = false;

  if(!initialised) 
  {
    Serial.begin(115200);
    initialised = true;
  }
  
  Serial.print(index);
  Serial.print(",");
  Serial.print(red_term);
  Serial.print(",");
  Serial.print(green_term);
  Serial.print(",");
  Serial.print(blue_term);
  Serial.println();
  delay(10);
}

static int get_distance_between(int idx1, int idx2)
{
  int distance = abs(idx2 - idx1);
  if (distance > (NUMPIXELS >> 1))
  {
    distance = NUMPIXELS - distance;
  }
  return distance;
}

static int gamma_adjust(int value, int gamma_8_8) 
{
  int distance = min(value, 255-value);
  int compensation = (distance * gamma_8_8) >> 8u;
  if (compensation > value)
  {
    value = 0;
  }
  else
  {
    value -= compensation;
  }
  return value;
}

static int get_red_component(int rainbow_position, int brightness_level)
{
  int component = get_colour_component(rainbow_position - 0, brightness_level);
  component = gamma_adjust(component, GAMMA_VALUE);
  return component;
}

static int get_green_component(int rainbow_position, int brightness_level)
{
  int component = get_colour_component(rainbow_position - 33, brightness_level);
  component = gamma_adjust(component, GAMMA_VALUE);
  /* Adjust for eye sensitivity to green */
  component = component >> 1;
  return component;
}

static int get_blue_component(int rainbow_position, int brightness_level)
{
  int component = get_colour_component(rainbow_position - 66, brightness_level);
  component = gamma_adjust(component, GAMMA_VALUE);
  return component;
}

static int get_colour_component(int rainbow_position, int brightness_level) {
  const int max_brightness = 100;
  const int max_rainbow_pos = 100;
  const int colour_extent = max_rainbow_pos / 3;
  const int half_colour_extent = colour_extent >> 1;
  const int max_colour_term = 255;
  int colour_term = 0;

  if (rainbow_position > max_rainbow_pos)
  {
    rainbow_position = max_rainbow_pos;
  }

  /* Wrap around the zero point */
  if (rainbow_position > (colour_extent << 1))
  {
    rainbow_position = rainbow_position - max_rainbow_pos;
  }
  
  /* Establish the colour weight */
  int colour_offset = abs(rainbow_position);
  if (colour_offset > colour_extent) 
  {
    colour_term = 0;
  }
  else if (colour_offset < half_colour_extent) 
  {
    colour_term = max_colour_term;
  }
  else 
  {
    int dimming_range_offset = colour_offset - half_colour_extent;
    colour_term = max_colour_term - ((max_colour_term  * dimming_range_offset) / half_colour_extent);
  }

  /* Saturate at the max value in case of overflow */
  if (colour_term > max_colour_term) 
  {
    colour_term = max_colour_term;
  }
  if (colour_term < 0) 
  {
    colour_term = 0;
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
