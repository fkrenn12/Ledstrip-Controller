#ifndef STRIP_H
#define STRIP_H

#include <NeoPixelBus.h>
#include <ArduinoJson.h>

#define DEFAULT_INTERVAL_MS 10000
#define DEFAULT_NUMBER_OF_PIXELS 10

class Neostrip {
private:
    const char* MODES[6] = {"off", "on", "rotate-left", "rotate-right", "shift-left", "shift-right"};
    int neo_pin;
    int number_of_pixels; 
    char* mode;
    //NeoPixelBus<NeoGrbFeature, NeoWs2812xMethod> strip(PixelCount, PixelPin);

public:
    // Konstruktor
    NeoPixelBus<NeoGrbFeature, NeoEsp32LcdX8Ws2812xMethod> strip; // note: modern WS2812 with letter like WS2812b
    Neostrip(int neo_pin, int interval = DEFAULT_INTERVAL_MS, int number_of_pixels = DEFAULT_NUMBER_OF_PIXELS);
    int interval;
    bool need_show; 
    unsigned long last_tick_ms;
    // Methoden
    void set_mode(const char* new_mode);
    void set_number_of_pixels(int new_number_of_pixels);
    void set_interval(int new_interval);
    void set_pattern(uint8_t* pattern, int pattern_length, int start = 0, int repeat = 0);
    void rotate_right();
    void rotate_left();
    void process_input(const char* input_data);
    void processing();
    void show();
};

#endif // STRIP_H


