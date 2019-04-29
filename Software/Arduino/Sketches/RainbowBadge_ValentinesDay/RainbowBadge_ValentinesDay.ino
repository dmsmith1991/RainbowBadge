#include <Bounce2.h>
#include <Metro.h>
#include <Adafruit_NeoPixel.h>

// #define SERIAL_DEBUG_ENABLED

enum HeartState_t
{
  FADE_IN,
  FADE_OUT
};

#define PIN_STATUS_LED 13
#define PIN_BUTTON     0

#define HEART_BRIGHTNESS_MIN    10
#define HEART_BRIGHTNESS_MAX    50
#define HEART_FADE_DELAY        50 // 30
#define HEART_TRANSITION_DELAY  100

#define POEM_START_DELAY   2000
#define POEM_TYPE_DELAY    100
#define POEM_NP_DELAY      500
#define POEM_DELAY_DIVIDER 4 // Heart pulsing is 4 times faster when poem is running.

#define HEART_NUM_RED_PIXELS 16
const byte HeartPattern[HEART_NUM_RED_PIXELS] = {1, 3, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 16, 17, 18, 22};

#define POEM_NUM_LINES 37
const String PoemLines[POEM_NUM_LINES] = {
  "",
  "",
  "   On Valentine's Day we think of those",
  "   Who make our lives worthwhile,",
  "   Those wonderful, heartwarming people who",
  "   We think of with a smile.",
  "",
  "   I am fortunate to know you,",
  "   That's why I want to say,",
  "   To a rare and special person:",
  "",
  "   Happy Valentine's Day!",
  "",
  "   From,",
  "   David",
  "",
  "            .     .",
  "       ..   8\"=,,88,   _.",
  "        8\"\"=\"\"8\'  \"88a88\'",
  "   .. .;88m a8   ,8\"\" \"8",
  "    \"8\"\'  \"88\"  A\"     8;",
  "      \"8,  \"8   8       \"8,",
  "       \"8   8,  8,       \"8",
  "        8,  \"8, \"8,    ___8,",
  "        \"8,  \"8, \"8mm\"\"\"\"\"\"8m.",
  "         \"8,am888i\"\'   ,mm\"",
  "         ,8\"  _8\"  .m888\"",
  "        ,88P\"\"\"\"\"I888888",
  "        \"\'         \"I888",
  "                      \"I8",
  "                       \"I8_",
  "           ,mmeem.m\"\"i, I8\"\"  ,mmeem,\'.",
  "          m\"\"    . \"8.8 I8  ,8\"   .  \"88",
  "         i8  . \'  ,mi\"\"8I8 ,8 . \'  ,8\"",
  "         88.\' ,mm\"\"    \"8I88\"m,,mm\'\"",
  "         \"8_m\"\"         \"I8   \"",
  ""
};

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(25, 12, NEO_GRB + NEO_KHZ800);
Bounce debouncer = Bounce();
Metro PoemMetro = Metro(POEM_TYPE_DELAY, true);
Metro HeartMetro = Metro(HEART_FADE_DELAY, true);

void setup()
{
  pinMode(PIN_STATUS_LED, OUTPUT);
  digitalWrite(PIN_STATUS_LED, LOW);
  
  pinMode(PIN_BUTTON, INPUT_PULLUP);
  debouncer.attach(PIN_BUTTON);
  debouncer.interval(20); // In Milliseconds
  
  pixels.begin();
  pixels.show();
  
  #ifdef SERIAL_DEBUG_ENABLED
  Serial.begin(9600);
  while(!Serial)
  {
    digitalWrite(PIN_STATUS_LED, HIGH);
  }
  digitalWrite(PIN_STATUS_LED, LOW);
  #endif
  
  delay(5000);
}

void loop()
{
  static HeartState_t heart_State = FADE_IN;
  
  static boolean  heart_RedPixelsDrawn = false;
  static byte     heart_CurrentBrightness = HEART_BRIGHTNESS_MIN;
  
  static boolean  poem_Running = true;
  static byte     poem_LineIndex = 0;
  static byte     poem_CharIndex = 0;
  static byte     poem_LineLength;
  static boolean  poem_KeyboardStarted = false;
  static boolean  poem_NotepadOpened = false;
  static boolean  poem_Waited = false;
  
  static unsigned long button_FallTime;
  
  /* Heart Code */
  if(!heart_RedPixelsDrawn)
  {
    for(byte i = 0; i < HEART_NUM_RED_PIXELS; i++)
    {
      pixels.setPixelColor(HeartPattern[i], 255, 0, 0);
    }
    heart_RedPixelsDrawn = true;
    
    #ifdef SERIAL_DEBUG_ENABLED
    Serial.println("Red Pixels Drawn!");
    #endif
  }
  
  if(HeartMetro.check()) // Has HeartMetro time elapsed?
  {
    switch(heart_State)
    {
      case FADE_IN:
        if(poem_Running) // Pulse heart faster if poem is running.
        {
          HeartMetro.interval(HEART_FADE_DELAY / POEM_DELAY_DIVIDER); // Set Metro interval to fade delay.
        }
        else
        {
          HeartMetro.interval(HEART_FADE_DELAY); // Set Metro interval to fade delay.
        }
      
        if(heart_CurrentBrightness <= HEART_BRIGHTNESS_MAX)
        {
          pixels.setBrightness(heart_CurrentBrightness);
          pixels.show();
          
          #ifdef SERIAL_DEBUG_ENABLED
          Serial.println(heart_CurrentBrightness);
          #endif
          
          heart_CurrentBrightness++;
        }
        else
        {
          heart_CurrentBrightness = HEART_BRIGHTNESS_MAX; // Ensure current brightness starts at max value.
          
          heart_State = FADE_OUT;
          
          if(poem_Running)
          {
            HeartMetro.interval(HEART_TRANSITION_DELAY / POEM_DELAY_DIVIDER); // Wait longer before transitioning.
          }
          else
          {
            HeartMetro.interval(HEART_TRANSITION_DELAY); // Wait longer before transitioning.
          }
          
          #ifdef SERIAL_DEBUG_ENABLED
          Serial.println("Transitioning...");
          #endif
        }
        break;
      case FADE_OUT:
        if(poem_Running) // Pulse heart faster if poem is running.
        {
          HeartMetro.interval(HEART_FADE_DELAY / POEM_DELAY_DIVIDER); // Set Metro interval to fade delay.
        }
        else
        {
          HeartMetro.interval(HEART_FADE_DELAY); // Set Metro interval to fade delay.
        }
      
        if(heart_CurrentBrightness >= HEART_BRIGHTNESS_MIN)
        {
          pixels.setBrightness(heart_CurrentBrightness);
          pixels.show();
          
          #ifdef SERIAL_DEBUG_ENABLED
          Serial.println(heart_CurrentBrightness);
          #endif
          
          heart_CurrentBrightness--;
        }
        else
        {
          heart_CurrentBrightness = HEART_BRIGHTNESS_MIN; // Ensure current brightness starts at min value.
          
          heart_State = FADE_IN;
          
          if(poem_Running)
          {
            HeartMetro.interval(HEART_TRANSITION_DELAY / POEM_DELAY_DIVIDER); // Wait longer before transitioning.
          }
          else
          {
            HeartMetro.interval(HEART_TRANSITION_DELAY); // Wait longer before transitioning.
          }
          
          #ifdef SERIAL_DEBUG_ENABLED
          Serial.println("Transitioning...");
          #endif
        }
        break;
    }
  }
  /* End of Heart Code */
  
  /* Poem Code */
  if(poem_Running)
  {
    if(!poem_KeyboardStarted) // Only start keyboard at beginning of play.
    {
      Keyboard.begin();
      delay(1000);
      
      digitalWrite(PIN_STATUS_LED, HIGH); // Indicate keyboard is being controlled by badge.
      
      poem_KeyboardStarted = true;
    }
    
    if(!poem_NotepadOpened) // Only open notepad once per play.
    {
      Keyboard.press(KEY_LEFT_GUI);
      delay(50);
      Keyboard.write('r');
      delay(50);
      Keyboard.release(KEY_LEFT_GUI);
      delay(1000);
      Keyboard.println("notepad");
      delay(2000);
      
      /* Maximise Window */
      Keyboard.press(KEY_LEFT_ALT);
      delay(50);
      Keyboard.write(' ');
      delay(50);
      Keyboard.release(KEY_LEFT_ALT);
      for(byte r = 0; r < 4; r++) // Down arrow 4 times.
      {
        Keyboard.press(KEY_DOWN_ARROW);
        delay(50);
        Keyboard.release(KEY_DOWN_ARROW);
        delay(50);
      }
      Keyboard.println(""); // Enter
      
      poem_NotepadOpened = true;
    }
    
    if(!poem_Waited) // One time wait before poem starts.
    {
      delay(POEM_START_DELAY);
      poem_Waited = true;
    }
    
    if(PoemMetro.check()) // Poem typing section.
    {
      if(poem_LineIndex < POEM_NUM_LINES) // Continue if there are still poem lines to type.
      {
        if(poem_CharIndex == 0)
        {
          poem_LineLength = PoemLines[poem_LineIndex].length();
        }
        if(poem_LineLength == 0) // If poem line is blank, start new paragraph.
        {
          Keyboard.println("");
          
          poem_CharIndex = 0; // Move to next poem line, as we've handled this one.
          poem_LineIndex++;
          
          delay(POEM_NP_DELAY);
        }
        else if(poem_CharIndex < poem_LineLength) // Type next character until end of current poem line is reached.
        {
          Keyboard.print(PoemLines[poem_LineIndex].charAt(poem_CharIndex));
          poem_CharIndex++;
        }
        else // Move to next poem line.
        {
          if(poem_LineIndex < (POEM_NUM_LINES - 1)) // Avoid new line on last poem line.
          {
            Keyboard.println("");
          }
          
          poem_CharIndex = 0;
          poem_LineIndex++;
        }
      }
      else // No more poem lines to type.
      {
        Keyboard.end(); // Release keyboard.
        
        digitalWrite(PIN_STATUS_LED, LOW); // Indicate keyboard has been released.
        
        /* Reset Required Variables */
        poem_Running = false;
        poem_LineIndex = 0;
        poem_CharIndex = 0;
        poem_KeyboardStarted = false;
        poem_NotepadOpened = false;
        poem_Waited = false;
      }
    }
  }
  /* End of Poem Code */
  
  /* Button Handling Code */
  if(!poem_Running)
  {
    debouncer.update();
    
    if(debouncer.fell())
    {
      button_FallTime = millis();
    }
    else if(debouncer.rose())
    {
      unsigned long timeDiff = millis() - button_FallTime;
      if(timeDiff > 2000) // Run poem if button was pressed for at least 2 seconds.
      {
        poem_Running = true;
      }
      if(timeDiff > 5000) // Don't open notepad if button was pressed for at least 5 seconds.
      {
        poem_NotepadOpened = true;
      }
    }
  }
  /* End of Button Handling Code */
}
