#include <stdint.h>

uint8_t extractAndScale(uint8_t value, uint8_t scaleFactor)
{
    return (value * 255) / scaleFactor;
}

void byteToRgb2222(uint8_t byteValue, uint8_t *r, uint8_t *g, uint8_t *b)
{
    *r = extractAndScale((byteValue >> 4) & 0b11, 16);
    *g = extractAndScale((byteValue >> 2) & 0b11, 16);
    *b = extractAndScale(byteValue & 0b11, 16);
    uint8_t brightness = ((byteValue >> 6) & 0b11) + 1;
    *r *= brightness;
    *g *= brightness;
    *b *= brightness;
}

uint8_t rgb2222ToByte(uint8_t brightness, uint8_t r, uint8_t g, uint8_t b)
{
    uint8_t mask = 0b00000011;
    r &= mask;
    g &= mask;
    b &= mask;
    brightness &= mask;
    return (brightness << 6) | (r << 4) | (g << 2) | b;
}