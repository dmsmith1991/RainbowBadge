#include <Bounce2.h>
#include <Adafruit_NeoPixel.h>

#define PIN_BUTTON 0

#define NUM_PIXELS 25

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUM_PIXELS, 12, NEO_GRB + NEO_KHZ800);
Bounce debouncer = Bounce();

#define NUM_FACES 3
#define FACE_HAPPY 0
#define FACE_OK 1
#define FACE_SAD 2

#define FACE_NUM_PIXELS 9

const byte Pattern_HappyFace[FACE_NUM_PIXELS] = {1, 3, 7, 10, 14, 16, 17, 18, 12};
const byte Pattern_OkFace[FACE_NUM_PIXELS] = {1, 3, 7, 15, 16, 17, 18, 19};
const byte Pattern_SadFace[FACE_NUM_PIXELS] = {1, 3, 7, 16, 17, 18, 20, 24};

void setup()
{
  pinMode(PIN_BUTTON, INPUT_PULLUP);
  debouncer.attach(PIN_BUTTON);
  debouncer.interval(100); // In Milliseconds
  
  pixels.begin();
  pixels.setBrightness(60);
  pixels.show(); // Initialise pixels to all off.
}

void loop()
{
  static byte currentFace = 0;
  static boolean refreshFace = true;
  
  if(refreshFace)
  {
    /* Blank All Pixels */
    for(byte i = 0; i < NUM_PIXELS; i++)
    {
      pixels.setPixelColor(i, 0, 0, 0);
    }
    
    /* Draw New Face */
    for(byte i = 0; i < FACE_NUM_PIXELS; i++)
    {
      switch(currentFace)
      {
        case FACE_HAPPY:
          pixels.setPixelColor(Pattern_HappyFace[i] - 1, 0, 255, 0);
          break;
        case FACE_OK:
          pixels.setPixelColor(Pattern_OkFace[i] - 1, 127, 127, 0);
          break;
        case FACE_SAD:
          pixels.setPixelColor(Pattern_SadFace[i] - 1, 255, 0, 0);
          break;
      }
    }
    
    pixels.show();
    
    refreshFace = false;
  }
  
  debouncer.update();
  if(debouncer.fell())
  {
    if(currentFace < (NUM_FACES - 1))
    {
      currentFace++;
    }
    else
    {
      currentFace = 0;
    }
    
    refreshFace = true;
  }
}
