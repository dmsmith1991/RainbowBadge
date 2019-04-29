/****************************************

Title Block Here

****************************************/

#ifndef RAINBOWBADGE_LEDMATRIX_H
#define RAINBOWBADGE_LEDMATRIX_H

#if (ARDUINO >= 100)
	#include <Arduino.h>
#else
	#include <WProgram.h>
	#include <pins_arduino.h>
#endif

#define MATRIX_PIN 12 // Arduino pin connected to pixel matrix.
#define MATRIX_NUM_PIXELS 25 // Number of pixels in matrix.
#define MATRIX_PIXEL_BUFFER_SIZE (MATRIX_NUM_PIXELS * 3) // Matrix pixel buffer size (3 bytes per pixel).

#define MATRIX_PIXEL_BUFFER_R_OFFSET 1 // Pixel buffer RGB offsets for GRB transmit order.
#define MATRIX_PIXEL_BUFFER_G_OFFSET 0
#define MATRIX_PIXEL_BUFFER_B_OFFSET 2

class RainbowBadge_LedMatrix
{
	public:
		RainbowBadge_LedMatrix();
		~RainbowBadge_LedMatrix();
		
		void begin(void);
		void show(void);
		void setPixelColour(uint8_t n, uint8_t r, uint8_t g, uint8_t b);
		void setPixelColour(uint8_t n, uint32_t c);
		static uint32_t RGBToColour(uint8_t r, uint8_t g, uint8_t b);
		uint32_t getPixelColour(uint8_t n) const;
		uint8_t numPixels(void) const;
		void setBrightness(uint8_t b);
		uint8_t getBrightness(void) const;
		void clear();
		
		inline bool canShow(void)
		{
			return (micros() - endTime) >= 50L;
		}
	
	private:
		uint8_t *pixels;
		uint8_t brightness;
		uint32_t endTime;
		const volatile uint8_t *port;
		uint8_t pinMask;
		
};

#endif // RAINBOWBADGE_LEDMATRIX_H