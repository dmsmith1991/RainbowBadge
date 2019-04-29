#include <RainbowBadge_LedMatrix.h>

RainbowBadge_LedMatrix matrix = RainbowBadge_LedMatrix(); // Create instance of RainbowBadge_LedMatrix class.

#define COLOUR_RED		matrix.RGBToColour(255, 0, 0) // Let's define some colours (from RGB values).
#define COLOUR_GREEN	matrix.RGBToColour(0, 255, 0)
#define COLOUR_BLUE		matrix.RGBToColour(0, 0, 255)
#define COLOUR_WHITE	matrix.RGBToColour(127, 127, 127)

void setup()
{
	matrix.begin(); // Begin control of matrix.
	matrix.show(); // Initialise all pixels to default state (off).
	
	Serial.begin(9600); // Start serial for outputting name of current effect.
}

void loop()
{
	// Some example functions showing how to write to the pixels.
	Serial.println("Red colour wipe...");
	colourWipe(COLOUR_RED, 50); // Red
	Serial.println("Green colour wipe...");
	colourWipe(COLOUR_GREEN, 50); // Green
	Serial.println("Blue colour wipe...");
	colourWipe(COLOUR_BLUE, 50); // Blue
	Serial.println("White colour wipe...");
	colourWipe(COLOUR_WHITE, 50); // White
	
	// Theatre chase effects.
	Serial.println("Red theatre chase...");
	theatreChase(COLOUR_RED, 50); // Red
	Serial.println("Green theatre chase...");
	theatreChase(COLOUR_GREEN, 50); // Green
	Serial.println("Blue theatre chase...");
	theatreChase(COLOUR_BLUE, 50); // Blue
	Serial.println("White theatre chase...");
	theatreChase(COLOUR_WHITE, 50); // White
	
	rainbow(20);
	rainbowCycle(20);
	theatreChaseRainbow(50);
}