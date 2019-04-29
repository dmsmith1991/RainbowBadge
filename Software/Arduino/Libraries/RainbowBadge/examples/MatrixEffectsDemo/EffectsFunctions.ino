/* Effects Functions */
void colourWipe(uint32_t c, byte wait)
{
	for(byte i = 0; i < matrix.numPixels(); i++) // Iterate through each pixel in matrix (sequentially).
	{
		matrix.setPixelColour(i, c); // Set colour of current pixel.
		matrix.show(); // Update matrix.
		delay(wait); // Wait for defined time before moving to next pixel (milliseconds).
	}
}

void rainbow(byte wait)
{
	for(byte j = 0; j < 256; j++)
	{
		for(byte i = 0; i < matrix.numPixels(); i++)
		{
			matrix.setPixelColour(i, wheel((i+j) & 255));
			matrix.show(); // Update matrix.
			delay(wait); // Wait for defined time before moving to next pixel (milliseconds).
		}
	}
}

void rainbowCycle(byte wait)
{
	for(unsigned int j = 0; j < (256 * 5); j++)
	{
		for(byte i = 0; i < matrix.numPixels(); i++)
		{
			matrix.setPixelColour(i, wheel(((i * 256 / matrix.numPixels()) + j) & 255));
		}
		matrix.show();
		delay(wait);
	}
}

void theatreChase(uint32_t c, byte wait)
{
	for(byte j = 0; j < 10; j++) // Do 10 cycles of chasing.
	{
		for(byte q = 0; q < 3; q++)
		{
			for(byte i = 0; i < matrix.numPixels(); i += 3)
			{
				matrix.setPixelColour(q + i, c); // Turn every third pixel on.
			}
			matrix.show();
			
			delay(wait);
			
			for(byte i = 0; i < matrix.numPixels(); i += 3)
			{
				matrix.setPixelColour(q + i, 0); // Turn every third pixel off.
			}
		}
	}
}

void theatreChaseRainbow(byte wait)
{
	for(byte j = 0; j < 256; j++) // Cycle all 256 colours in the wheel.
	{
		for(byte q = 0; q < 3; q++)
		{
			for(byte i = 0; i < matrix.numPixels(); i += 3)
			{
				matrix.setPixelColour(q + i, wheel((i + j) % 255)); // Turn every third pixel on.
			}
			matrix.show();
			
			delay(wait);
			
			for(byte i = 0; i < matrix.numPixels(); i += 3)
			{
				matrix.setPixelColour(q + i, 0); // Turn every third pixel off.
			}
		}
	}
}
/* End Of Effects Functions */

/* Helper Functions */
uint32_t wheel(byte wheelPos)
{
	wheelPos = 255 - wheelPos;
	if(wheelPos < 85)
	{
		return matrix.RGBToColour(255 - wheelPos * 3, 0, wheelPos * 3);
	}
	else if(wheelPos < 170)
	{
		wheelPos -= 85;
		return matrix.RGBToColour(0, wheelPos * 3, 255 - wheelPos * 3);
	}
	else
	{
		wheelPos -= 170;
		return matrix.RGBToColour(wheelPos * 3, 255 - wheelPos * 3, 0);
	}
}
/* End Of Helper Functions */