#include <stdint.h>
#include <Arduino.h>

const u8_t color_table[] = {0, 15, 30, 63}; // Maximum number of pixels per strip

/*
uint8_t extractAndScale(uint8_t value, uint8_t scaleFactor)
{
    return (value * 255) / scaleFactor;
}
*/

/**
Converts an 8-bit byte value into RGB2222 color values and modifies the given pointers with the scaled RGB components.

@param byteValue The 8-bit input representing color and brightness information.
@param r Pointer to store the red component (scaled by brightness).
@param g Pointer to store the green component (scaled by brightness).
@param b Pointer to store the blue component (scaled by brightness).
 */
void byteToRgb2222(uint8_t byteValue, uint8_t *r, uint8_t *g, uint8_t *b)
{
    // *r = extractAndScale((byteValue >> 4) & 0b11, 16);
    // *g = extractAndScale((byteValue >> 2) & 0b11, 16);
    // *b = extractAndScale(byteValue & 0b11, 16);
    *r = color_table[(byteValue >> 4) & 0b11];
    *g = color_table[(byteValue >> 2) & 0b11];
    *b = color_table[byteValue & 0b11];
    uint8_t brightness = ((byteValue >> 6) & 0b11) + 1;
    *r *= brightness;
    *g *= brightness;
    *b *= brightness;   
}

/*
uint8_t rgb2222ToByte(uint8_t brightness, uint8_t r, uint8_t g, uint8_t b)
{
    uint8_t mask = 0b00000011;
    r &= mask;
    g &= mask;
    b &= mask;
    brightness &= mask;
    return (brightness << 6) | (r << 4) | (g << 2) | b;
}
*/