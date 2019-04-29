#include <Metro.h>
#include <Bounce2.h>
#include <Adafruit_NeoPixel.h>

#define PIN_BUTTON     0
#define PIN_NEOPIXELS  12
#define PIN_LED        13

#define NUM_PIXELS 25

Bounce debouncer = Bounce();
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUM_PIXELS, PIN_NEOPIXELS, NEO_GRB + NEO_KHZ800);
Metro LedUpdateMetro = Metro(1000 / (10 * 2));

boolean LedState = LOW;
byte    CurrentPixel = 0;

void setup()
{
  pinMode(PIN_BUTTON, INPUT_PULLUP);
  
  debouncer.attach(PIN_BUTTON);
  debouncer.interval(50); // In Milliseconds.
  
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LOW);
  
  pixels.begin();
  pixels.setBrightness(60);
}

void loop()
{
  debouncer.update(); // Call periodically.
  
  /*
  if(debouncer.fell()) // Falling edge on button detected?
  {
    LedState = !LedState;
    digitalWrite(PIN_LED, LedState);
  }
  */
  
  if(debouncer.read() == LOW)
  {
    digitalWrite(PIN_LED, HIGH);
  }
  else
  {
    digitalWrite(PIN_LED, LOW);
  }
  
  if(LedUpdateMetro.check())
  {
    /* All Neopixels Off */
    for(byte i = 0; i < NUM_PIXELS; i++)
    {
      pixels.setPixelColor(i, 0, 0, 0);
    }
    
    /* Illuminate Next Neopixel Red */
    pixels.setPixelColor(CurrentPixel, 255, 0, 0);
    if(CurrentPixel < (NUM_PIXELS - 1))
    {
      CurrentPixel++; // Increment for next time.
    }
    else
    {
      CurrentPixel = 0;
    }
    
    pixels.show();
  }
}
