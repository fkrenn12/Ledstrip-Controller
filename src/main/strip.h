#ifndef STRIP_H
#define STRIP_H

#include <Adafruit_NeoPixel.h>
#include <NeoPixelBus.h>
#include <ArduinoJson.h>

#define DEFAULT_INTERVAL_MS 1000
#define DEFAULT_NUMBER_OF_PIXELS 10

class Neostrip {
private:
    const char* MODES[4] = {"off", "on", "left", "right"};
    int neo_pin;
    int number_of_pixels;
    int interval;
    unsigned long last_tick_ms;
    const char* mode;
    Adafruit_NeoPixel pixels;
    //NeoPixelBus<NeoGrbFeature, NeoWs2812xMethod> strip(PixelCount, PixelPin);

public:
    // Konstruktor
    Neostrip(int neo_pin, int interval = DEFAULT_INTERVAL_MS, int number_of_pixels = DEFAULT_NUMBER_OF_PIXELS);

    // Methoden
    void set_mode(const char* new_mode);
    void set_number_of_pixels(int new_number_of_pixels);
    void set_interval(int new_interval);
    void set_pattern(uint8_t* pattern, int pattern_length, int start = 0, int repeat = 0);
    void rotate_right();
    void rotate_left();
    void process_input(const char* input_data);
    void processing();
};

#endif // STRIP_H


