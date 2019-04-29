/****************************************

Title Block Here

****************************************/

#include "RainbowBadge_LedMatrix.h"

/*
Class constructor.
*/
RainbowBadge_LedMatrix::RainbowBadge_LedMatrix() : pixels(NULL), brightness(0), endTime(0), port(portOutputRegister(digitalPinToPort(MATRIX_PIN))), pinMask(digitalPinToBitMask(MATRIX_PIN))
{
	if((pixels = (uint8_t *)malloc(MATRIX_PIXEL_BUFFER_SIZE))) // Allocate RAM for pixel buffer, initialise if successful.
	{
		memset(pixels, 0, MATRIX_PIXEL_BUFFER_SIZE);
	}
	
	setBrightness(40); // Set default brightness.
}

/*
Class de-constructor.
*/
RainbowBadge_LedMatrix::~RainbowBadge_LedMatrix()
{
	if(pixels) free(pixels); // Release pixel buffer RAM.
	pinMode(MATRIX_PIN, INPUT); // Return matrix pin to input.
}

void RainbowBadge_LedMatrix::begin(void)
{
	pinMode(MATRIX_PIN, OUTPUT); // Set matrix pin to output.
	digitalWrite(MATRIX_PIN, LOW); // Set matrix pin low.
}

/*
Push contents of pixel buffer to pixels.
*/
void RainbowBadge_LedMatrix::show(void)
{
	if(!pixels) return; // Exit function if pixel buffer isn't initialised.
	
	while(!canShow()); // Give pixels time to update from any previous show calls.
	
	noInterrupts(); // Need 100% focus on instruction timing.
	
	volatile uint16_t i = MATRIX_PIXEL_BUFFER_SIZE; // Loop counter.
	volatile uint8_t *ptr = pixels; // Pointer to next byte.
	volatile uint8_t b   = *ptr++; // Current byte value.
	volatile uint8_t hi; // PORT w/output bit set high.
	volatile uint8_t lo; // PORT w/output bit set low.
	volatile uint8_t n1, n2 = 0;  // First, next bits out
	
	#if (F_CPU >= 7400000UL) && (F_CPU <= 9500000UL) // Routine for 8MHz clock speed (ish).
		hi = PORTD | pinMask;
		lo = PORTD & ~pinMask;
		n1 = lo;
		if(b & 0x80) n1 = hi;
		
		asm volatile(
		"headD:"                   "\n\t" // Clk  Pseudocode
		// Bit 7:
		"out  %[port] , %[hi]"    "\n\t" // 1    PORT = hi
		"mov  %[n2]   , %[lo]"    "\n\t" // 1    n2   = lo
		"out  %[port] , %[n1]"    "\n\t" // 1    PORT = n1
		"rjmp .+0"                "\n\t" // 2    nop nop
		"sbrc %[byte] , 6"        "\n\t" // 1-2  if(b & 0x40)
		"mov %[n2]   , %[hi]"    "\n\t" // 0-1   n2 = hi
		"out  %[port] , %[lo]"    "\n\t" // 1    PORT = lo
		"rjmp .+0"                "\n\t" // 2    nop nop
		// Bit 6:
		"out  %[port] , %[hi]"    "\n\t" // 1    PORT = hi
		"mov  %[n1]   , %[lo]"    "\n\t" // 1    n1   = lo
		"out  %[port] , %[n2]"    "\n\t" // 1    PORT = n2
		"rjmp .+0"                "\n\t" // 2    nop nop
		"sbrc %[byte] , 5"        "\n\t" // 1-2  if(b & 0x20)
		"mov %[n1]   , %[hi]"    "\n\t" // 0-1   n1 = hi
		"out  %[port] , %[lo]"    "\n\t" // 1    PORT = lo
		"rjmp .+0"                "\n\t" // 2    nop nop
		// Bit 5:
		"out  %[port] , %[hi]"    "\n\t" // 1    PORT = hi
		"mov  %[n2]   , %[lo]"    "\n\t" // 1    n2   = lo
		"out  %[port] , %[n1]"    "\n\t" // 1    PORT = n1
		"rjmp .+0"                "\n\t" // 2    nop nop
		"sbrc %[byte] , 4"        "\n\t" // 1-2  if(b & 0x10)
		"mov %[n2]   , %[hi]"    "\n\t" // 0-1   n2 = hi
		"out  %[port] , %[lo]"    "\n\t" // 1    PORT = lo
		"rjmp .+0"                "\n\t" // 2    nop nop
		// Bit 4:
		"out  %[port] , %[hi]"    "\n\t" // 1    PORT = hi
		"mov  %[n1]   , %[lo]"    "\n\t" // 1    n1   = lo
		"out  %[port] , %[n2]"    "\n\t" // 1    PORT = n2
		"rjmp .+0"                "\n\t" // 2    nop nop
		"sbrc %[byte] , 3"        "\n\t" // 1-2  if(b & 0x08)
		"mov %[n1]   , %[hi]"    "\n\t" // 0-1   n1 = hi
		"out  %[port] , %[lo]"    "\n\t" // 1    PORT = lo
		"rjmp .+0"                "\n\t" // 2    nop nop
		// Bit 3:
		"out  %[port] , %[hi]"    "\n\t" // 1    PORT = hi
		"mov  %[n2]   , %[lo]"    "\n\t" // 1    n2   = lo
		"out  %[port] , %[n1]"    "\n\t" // 1    PORT = n1
		"rjmp .+0"                "\n\t" // 2    nop nop
		"sbrc %[byte] , 2"        "\n\t" // 1-2  if(b & 0x04)
		"mov %[n2]   , %[hi]"    "\n\t" // 0-1   n2 = hi
		"out  %[port] , %[lo]"    "\n\t" // 1    PORT = lo
		"rjmp .+0"                "\n\t" // 2    nop nop
		// Bit 2:
		"out  %[port] , %[hi]"    "\n\t" // 1    PORT = hi
		"mov  %[n1]   , %[lo]"    "\n\t" // 1    n1   = lo
		"out  %[port] , %[n2]"    "\n\t" // 1    PORT = n2
		"rjmp .+0"                "\n\t" // 2    nop nop
		"sbrc %[byte] , 1"        "\n\t" // 1-2  if(b & 0x02)
		"mov %[n1]   , %[hi]"    "\n\t" // 0-1   n1 = hi
		"out  %[port] , %[lo]"    "\n\t" // 1    PORT = lo
		"rjmp .+0"                "\n\t" // 2    nop nop
		// Bit 1:
		"out  %[port] , %[hi]"    "\n\t" // 1    PORT = hi
		"mov  %[n2]   , %[lo]"    "\n\t" // 1    n2   = lo
		"out  %[port] , %[n1]"    "\n\t" // 1    PORT = n1
		"rjmp .+0"                "\n\t" // 2    nop nop
		"sbrc %[byte] , 0"        "\n\t" // 1-2  if(b & 0x01)
		"mov %[n2]   , %[hi]"    "\n\t" // 0-1   n2 = hi
		"out  %[port] , %[lo]"    "\n\t" // 1    PORT = lo
		"sbiw %[count], 1"        "\n\t" // 2    i-- (don't act on Z flag yet)
		// Bit 0:
		"out  %[port] , %[hi]"    "\n\t" // 1    PORT = hi
		"mov  %[n1]   , %[lo]"    "\n\t" // 1    n1   = lo
		"out  %[port] , %[n2]"    "\n\t" // 1    PORT = n2
		"ld   %[byte] , %a[ptr]+" "\n\t" // 2    b = *ptr++
		"sbrc %[byte] , 7"        "\n\t" // 1-2  if(b & 0x80)
		"mov %[n1]   , %[hi]"    "\n\t" // 0-1   n1 = hi
		"out  %[port] , %[lo]"    "\n\t" // 1    PORT = lo
		"brne headD"              "\n"   // 2    while(i) (Z flag set above)
		: [byte]  "+r" (b),
		[n1]    "+r" (n1),
		[n2]    "+r" (n2),
		[count] "+w" (i)
		: [port]   "I" (_SFR_IO_ADDR(PORTD)),
		[ptr]    "e" (ptr),
		[hi]     "r" (hi),
		[lo]     "r" (lo));
	#else
		#error "CPU SPEED NOT SUPPORTED!"
	#endif
	
	interrupts(); // Restore interrupts.
	endTime = micros(); // Save EOD time for latch on next call.
}

/*
Set pixel colour from separate RGB values.
*/
void RainbowBadge_LedMatrix::setPixelColour(uint8_t n, uint8_t r, uint8_t g, uint8_t b)
{
	if(n < MATRIX_NUM_PIXELS)
	{
		if(brightness) // See notes in setBrightness()
		{
			r = (r * brightness) >> 8;
			g = (g * brightness) >> 8;
			b = (b * brightness) >> 8;
		}
		
		uint8_t *p = &pixels[n * 3];
		p[MATRIX_PIXEL_BUFFER_R_OFFSET] = r;
		p[MATRIX_PIXEL_BUFFER_G_OFFSET] = g;
		p[MATRIX_PIXEL_BUFFER_B_OFFSET] = b;
	}
}

/*
Set pixel colour from 'packed' 32-bit RGB colour value.
*/
void RainbowBadge_LedMatrix::setPixelColour(uint8_t n, uint32_t c)
{
	if(n < MATRIX_NUM_PIXELS)
	{
		uint8_t r = (uint8_t)(c >> 16);
		uint8_t g = (uint8_t)(c >> 8);
		uint8_t b = (uint8_t)c;
		
		if(brightness) // See notes in setBrightness()
		{
			r = (r * brightness) >> 8;
			g = (g * brightness) >> 8;
			b = (b * brightness) >> 8;
		}
		
		uint8_t *p = &pixels[n * 3];
		p[MATRIX_PIXEL_BUFFER_R_OFFSET] = r;
		p[MATRIX_PIXEL_BUFFER_G_OFFSET] = g;
		p[MATRIX_PIXEL_BUFFER_B_OFFSET] = b;
	}
}

/*
Convert separate R,G,B values into packed 32-bit RGB colour.
Packed format is always RGB, regardless of specific pixel colour order.
*/
uint32_t RainbowBadge_LedMatrix::RGBToColour(uint8_t r, uint8_t g, uint8_t b)
{
	return ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
}

/*
Query colour from previously-set pixel (returns packed 32-bit RGB value).
*/
uint32_t RainbowBadge_LedMatrix::getPixelColour(uint8_t n) const
{
	if(n >= MATRIX_NUM_PIXELS)
	{
		return 0; // Out of bounds, return no colour.
	}
	
	uint8_t *p = &pixels[n * 3];
	uint32_t c = ((uint32_t)p[MATRIX_PIXEL_BUFFER_R_OFFSET] << 16) | ((uint32_t)p[MATRIX_PIXEL_BUFFER_G_OFFSET] <<  8) | (uint32_t)p[MATRIX_PIXEL_BUFFER_B_OFFSET];
	
	// Adjust this back up to the true colour, as setting a pixel colour will scale it back down again.
	if(brightness) // See notes in setBrightness()
	{
		// Cast the colour to a byte array
		uint8_t * c_ptr =reinterpret_cast<uint8_t*>(&c);
		c_ptr[0] = (c_ptr[0] << 8) / brightness;
		c_ptr[1] = (c_ptr[1] << 8) / brightness;
		c_ptr[2] = (c_ptr[2] << 8) / brightness;
	}
	
	return c;
}

uint8_t RainbowBadge_LedMatrix::numPixels(void) const
{
	return MATRIX_NUM_PIXELS;
}

void RainbowBadge_LedMatrix::setBrightness(uint8_t b)
{
	uint8_t newBrightness = b + 1;
	if(newBrightness != brightness) // Compare against prior value
	{
		// Brightness has changed -- re-scale existing data in RAM
		uint8_t c;
		uint8_t *ptr = pixels;
		uint8_t oldBrightness = brightness - 1; // De-wrap old brightness value
		uint16_t scale;
		
		if(oldBrightness == 0)
		{
			scale = 0; // Avoid /0
		}
		else if(b == 255)
		{
			scale = 65535 / oldBrightness;
		}
		else
		{
			scale = (((uint16_t)newBrightness << 8) - 1) / oldBrightness;
		}
		
		for(uint8_t i = 0; i < MATRIX_PIXEL_BUFFER_SIZE; i++)
		{
			c = *ptr;
			*ptr++ = (c * scale) >> 8;
		}
		brightness = newBrightness;
	}
}

uint8_t RainbowBadge_LedMatrix::getBrightness(void) const
{
	return brightness - 1;
}

void RainbowBadge_LedMatrix::clear()
{
	memset(pixels, 0, MATRIX_PIXEL_BUFFER_SIZE);
}

// Continue here...