#include <Adafruit_NeoPixel.h>
#include <Metro.h>

#define PIN_PIXELS 12

#define NUM_PIXELS 25
#define PATTERN_NUM_PIXELS 9
const byte Pattern[PATTERN_NUM_PIXELS] = {2, 6, 7, 8, 10, 12, 14, 17, 22};

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUM_PIXELS, PIN_PIXELS, NEO_GRB + NEO_KHZ800);

void setup()
{
  pixels.begin();
  pixels.setBrightness(60);
  
  /* Set Pattern Pixels */
  for(byte i = 0; i < PATTERN_NUM_PIXELS; i++)
  {
    pixels.setPixelColor(Pattern[i] - 1, 255, 0, 0);
  }
  
   pixels.show();
}

void loop()
{
  
}
