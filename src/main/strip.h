#ifndef STRIP_H
#define STRIP_H

#include <NeoPixelBus.h>
#include <ArduinoJson.h>

#define MAX_PIXEL_PER_STRIP 1000
#define DEFAULT_INTERVAL_MS 10000
#define DEFAULT_NUMBER_OF_PIXELS 10

class Neostrip {
private:
    String ANIMATION_MODES[6] = {"off", "rotate-left", "rotate-right", "shift-left", "shift-right"};
    String UPDATE_MODES[3] = {"instant", "triggered"};
    u8_t neo_pin;
    u16_t number_of_pixels; 
    u8_t shadow_strip[1000] ={0};  
    bool shadow_strip_dirty;
    String animation_mode;
    //NeoPixelBus<NeoGrbFeature, NeoWs2812xMethod> strip(PixelCount, PixelPin);
public:
    // Konstruktor
    NeoPixelBus<NeoGrbFeature, NeoEsp32LcdX8Ws2812xMethod> strip; // note: modern WS2812 with letter like WS2812b
    Neostrip(u8_t neo_pin, u16_t interval = DEFAULT_INTERVAL_MS, u16_t number_of_pixels = DEFAULT_NUMBER_OF_PIXELS);
    u16_t interval;
    bool need_show; 
    u64_t last_tick_ms;
    String update_mode;
    // Methoden
    void set_animation_mode(String new_mode);
    void set_update_mode(String new_mode);
    void set_number_of_pixels(u16_t new_number_of_pixels);
    void set_interval(u16_t new_interval);
    //void set_pattern(uint8_t* pattern, int pattern_length, int start = 0, int repeat = 0, int add = 0);
    void set_pattern(JsonArray pattern, u16_t start = 0, u16_t repeat = 0, u16_t add = 0);
    void process_input(JsonDocument doc);
    void animate();
    void transfer_shadow_into_strip_if_dirty();
    void show();
};

#endif // STRIP_H

