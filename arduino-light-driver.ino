#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif
#define DATA_PIN  6
#define NUMPIXELS 48
#define LED_PIN 13
#define DELAYVAL 500
Adafruit_NeoPixel pixels(NUMPIXELS, DATA_PIN, NEO_GRB + NEO_KHZ800);



void setup() {
  // put your setup code here, to run once:
  pixels.begin();
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  static int cycle_counter = 0;
  // put your main code here, to run repeatedly:
  pixels.clear();

  cycle_counter += 1;
  digitalWrite(LED_PIN, HIGH);
  delay(DELAYVAL);
  digitalWrite(LED_PIN, LOW);
  delay(DELAYVAL);

  for(int i=0; i<NUMPIXELS; i++) 
  {
    int offset = (i + cycle_counter) & 0x7;

    
    if (offset < 1 )
    { 
      pixels.setPixelColor(i, pixels.Color(150, 150, 150));
    } else if (offset < 2 )
    { 
      pixels.setPixelColor(i, pixels.Color(150, 0, 0));
    } else if (offset < 3 )
    { 
      pixels.setPixelColor(i, pixels.Color(150, 150, 0));
    } else if (offset < 4 )
    { 
      pixels.setPixelColor(i, pixels.Color(0, 150, 0));
    } else if (offset < 5 )
    { 
      pixels.setPixelColor(i, pixels.Color(0, 150, 150));
    } else if (offset < 6 ) 
    { 
      pixels.setPixelColor(i, pixels.Color(0, 0, 150));
    } 
    else 
    { 
      pixels.setPixelColor(i, pixels.Color(0, 0, 0));
    }
  }
  pixels.show();
}
