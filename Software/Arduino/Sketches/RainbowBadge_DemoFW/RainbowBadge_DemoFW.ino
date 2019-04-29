// #define DEBUG_MODE

#include <Metro.h>
#include <Adafruit_NeoPixel.h>

Adafruit_NeoPixel Pixels = Adafruit_NeoPixel(25, 12, NEO_GRB + NEO_KHZ800);
Metro LedMetro = Metro(100);

#define PIN_LED 13

#define FIRMWARE_INFO_STR F("RainbowBadgeDemoFW V0.01")

#define CMD_BUFFER_SIZE 20 // Make as small as possible.
#define INDEX_MAX_VALUE 24

enum SetCommands_t
{ 
  SetEvent,
  SetFramePointer,
  SetNumFrames,
  SetLedColour,
  SetGlobalBrightness
};

struct ProcessResults_t
{
  SetCommands_t Cmd;
  byte Index;
  byte Values[3];
  byte NumValues;
};

char CmdBuffer[CMD_BUFFER_SIZE];
byte CmdBufferPos = 0;
ProcessResults_t CmdBufferProcessResults;
boolean NewLineRequired = false;
boolean ErrorFlag = false;

void setup()
{
  Pixels.begin();
  Pixels.setBrightness(60);
  Pixels.show(); // Initialise pixels to all off.
  
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LOW);
  
  Serial.begin(9600);
}

void loop()
{
  static byte ledState = LOW;
  
  if(!Serial)
  {
    /* Normal Operation Code */
    LedMetro.interval(500);
    if(LedMetro.check())
    {
      ledState == HIGH ? ledState = LOW : ledState = HIGH;
      digitalWrite(PIN_LED, ledState);
    }
    /* End Of Normal Operation Code */
  }
  else
  {
    LedMetro.interval(100);
    if(LedMetro.check())
    {
      ledState == HIGH ? ledState = LOW : ledState = HIGH;
      digitalWrite(PIN_LED, ledState);
    }
    
    if(Serial.available() > 0)
    {
      char receivedChar = Serial.read();
  
      if(IsExpectedCmdChar(receivedChar) || receivedChar == '\r' || receivedChar == '\n') // Only continue if received char is expected.
      {
        if(receivedChar == '-') // Clear command buffer.
        {
          CmdBufferPos = 0;
          PrintNewLineIfRequired();
          Serial.println("BUFFER_CLEARED");
        }
        else if(receivedChar == '?') // Print firmware info.
        {
          PrintNewLineIfRequired();
          Serial.println(FIRMWARE_INFO_STR);
        }
        else if(receivedChar != '\r' && receivedChar != '\n') // Normal char received.
        {
          /* Add Received Char To Command Buffer */
          if(CmdBufferPos < CMD_BUFFER_SIZE)
          {
            CmdBuffer[CmdBufferPos] = receivedChar;
            Serial.print(CmdBuffer[CmdBufferPos]); // Echo back character to indicate that it was excepted.
            CmdBufferPos++;
  
            NewLineRequired = true;
          }
        }
        else // Newline char received, so parse command buffer.
        {
          PrintNewLineIfRequired();
  
          if(CmdBufferPos > 0) // Skip if command buffer is empty.
          {
            if(CmdBuffer_Process())
            {
              Serial.println("OK");
              
              #ifdef DEBUG_MODE
              Serial.println("############################################");
              switch(CmdBufferProcessResults.Cmd)
              {
                case SetEvent:
                  Serial.println("Command: SetEvent");
                  Serial.print("Index: ");
                  Serial.println(CmdBufferProcessResults.Index);
                  break;
                case SetFramePointer:
                  Serial.println("Command: SetFramePointer");
                  break;
                case SetNumFrames:
                  Serial.println("Command: SetNumFrames");
                  break;
                case SetGlobalBrightness:
                  Serial.println("Command: SetGlobalBrightness");
                  break;
                case SetLedColour:
                  Serial.println("Command: SetLedColour");
                  Serial.print("Index: ");
                  Serial.println(CmdBufferProcessResults.Index);
                  break;
              }
              for(byte i = 0; i < CmdBufferProcessResults.NumValues; i++)
              {
                Serial.print("Value ");
                Serial.print(i + 1);
                Serial.print(": ");
                Serial.println(CmdBufferProcessResults.Values[i]);
              }
              Serial.println("############################################");
              #endif
              
              switch(CmdBufferProcessResults.Cmd)
              {
                case SetLedColour:
                  Pixels.setPixelColor(CmdBufferProcessResults.Index, CmdBufferProcessResults.Values[0], CmdBufferProcessResults.Values[1], CmdBufferProcessResults.Values[2]);
                  Pixels.show();
                  break;
                case SetGlobalBrightness:
                  Pixels.setBrightness(CmdBufferProcessResults.Values[0]);
                  Pixels.show();
                  break;
              }
            }
            else
            {
              Serial.println("SYNTAX_ERROR");
            }
  
            CmdBufferPos = 0;
          }
        }
      }
    }
  }
}

boolean CmdBuffer_Process(void)
{
  SetCommands_t cmd;
  boolean expectingIndex;
  byte expectedNumValues;
  byte index = 0xFF;
  byte values[3];

  switch(CmdBuffer[0]) // Check command keychar.
  {
    case 'E': // Set event.
      cmd = SetEvent;
      expectingIndex = true;
      expectedNumValues = 2;
      break;
    case 'F': // Set frame pointer.
      cmd = SetFramePointer;
      expectingIndex = false;
      expectedNumValues = 1;
      break;
    case 'N': // Set number of frames.
      cmd = SetNumFrames;
      expectingIndex = false;
      expectedNumValues = 1;
      break;
    case 'L': // Set LED colour for current frame.
      cmd = SetLedColour;
      expectingIndex = true;
      expectedNumValues = 3;
      break;
    case 'B': // Set global (across all frames) LED brightness.
      cmd = SetGlobalBrightness;
      expectingIndex = false;
      expectedNumValues = 1;
      break;
    default: // Unknown command keychar, return now.
    return false;
  }

  /* If Command Has Index, Extract And Store */
  if(expectingIndex)
  {
    // Serial.println("############### INDEX ###############");
    
    if(CmdBuffer[1] == '[') // Check second char is opening bracket.
    {
      byte charPos = FindChar(']'); // Find position of closing bracket.
      if(charPos != 0xFF) // Closing bracket found? (0xFF returned if not)
      {
        index = CmdBuffer_ParseByte(2, charPos - 1);
        if(ErrorFlag || index > INDEX_MAX_VALUE)
        {
          return false;
        }
      }
      else
      {
        return false;
      }
    }
    else
    {
      return false;
    }
  }
  
  byte charPos = FindChar('=');
  if(charPos != 0xFF) // Equals sign found? (0xFF returned if not)
  {
    if(expectingIndex)
    {
      if(CmdBuffer[charPos - 1] != ']')
      {
        return false;
      }
    }
    else
    {
      if(charPos != 1)
      {
        return false;
      }
    }
    
    /* Extract Values */
    byte currentStartPos = charPos + 1;
    byte currentEndPos;
    for(byte i = 0; i < expectedNumValues; i++)
    {
      /*
      Serial.print("############### VALUE ");
      Serial.print(i + 1);
      Serial.println(" ###############");
      */
      
      if(i > 0)
      {
        currentStartPos = currentEndPos + 2;
      }
      
      if(i == (expectedNumValues - 1)) // On last value?
      {
        currentEndPos = CmdBufferPos - 1;
      }
      else // Not on last value.
      {
        charPos = FindChar(',', currentStartPos);
        if(charPos != 0xFF) // Comma found? (0xFF returned if not)
        {
          currentEndPos = charPos - 1;
        }
        else
        {
          return false;
        }
      }
      
      values[i] = CmdBuffer_ParseByte(currentStartPos, currentEndPos);
      if(ErrorFlag)
      {
        return false;
      }
    }
  }
  else
  {
    return false;
  }
  
  CmdBufferProcessResults.Cmd = cmd;
  CmdBufferProcessResults.Index = index;
  CmdBufferProcessResults.NumValues = expectedNumValues;
  for(byte i = 0; i < 3; i++)
  {
    CmdBufferProcessResults.Values[i] = values[i];
  }

  return true;
}

byte CmdBuffer_ParseByte(byte startPos, byte endPos)
{
  ErrorFlag = false;
  
  int workingVal = 0;
  byte numLength = (endPos - startPos) + 1;
  
  /*
  Serial.print("numLength:");
  Serial.println(numLength);
  */

  if(numLength > 0 && numLength <= 3) // Number string length between 1 and 3?
  {
    byte multiplier = 1;
    for(byte r = 1; r < numLength; r++)
    {
      multiplier *= 10;
    }

    for(byte i = startPos; i <= endPos; i++) // Iterate though number digits.
    {
      if(isDigit(CmdBuffer[i]))
      {
        byte n = (byte)CmdBuffer[i] - 48; // Convert number char to numeric byte.
        workingVal += (n * multiplier);
        
        /*
        Serial.print("multiplier:");
        Serial.println(multiplier);
        Serial.print("n:");
        Serial.println(n);
        Serial.print("workingVal:");
        Serial.println(workingVal);
        */
        
        multiplier /= 10;
      }
      else // Current char is a non-digit, so return now.
      {
        ErrorFlag = true;
        return 0;
      }
    }
    
    if(workingVal <= 0xFF) // Returned value no larger than a byte?
    {
      return lowByte(workingVal); // Return only the low byte.
    }
    else // Returned value is not a byte, so return now.
    {
      ErrorFlag = true;
      return 0;
    }
  }
  else
  {
    ErrorFlag = true;
    return 0;
  }
}

byte FindChar(char c)
{
  for(byte i = 0; i < CmdBufferPos; i++)
  {
    if(CmdBuffer[i] == c)
    {
      return i;
    }
  }
  
  return 0xFF;
}

byte FindChar(char c, byte startPos)
{
  for(byte i = startPos; i < CmdBufferPos; i++)
  {
    if(CmdBuffer[i] == c)
    {
      return i;
    }
  }
  
  return 0xFF;
}

boolean IsExpectedCmdChar(char c)
{
  #define NUM_CHARS 21
  const char expectedChars[NUM_CHARS] = {'-', '?', 'E', 'F', 'N', 'L', 'B', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '[', ']', '=', ','};

  for(byte i = 0; i < NUM_CHARS; i++)
  {
    if(c == expectedChars[i])
    {
      return true;
    }
  }

  return false;
}

void PrintNewLineIfRequired(void)
{
  if(NewLineRequired)
  {
    Serial.println("");
    NewLineRequired = false;
  }
}
