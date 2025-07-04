#include "strip.h"
#include "rgb.h"
#include <ArduinoJson.h>
#include <NeoPixelBus.h>


// Konstruktor
Neostrip::Neostrip(int neo_pin, int interval, int number_of_pixels)
    : neo_pin(neo_pin), number_of_pixels(number_of_pixels), interval(interval), mode("on"), last_tick_ms(0),
      strip(number_of_pixels, neo_pin){
    // pixels.begin();
    // pixels.fill(pixels.Color(10, 0, 0));
    // pixels.show();
    strip.Begin();
    strip.ClearTo(RgbColor(0, 0, 0));
}

// Modus setzen
void Neostrip::set_mode(const char* new_mode) {
    for (const char* valid_mode : MODES) {
        if (strcmp(new_mode, valid_mode) == 0) {
            mode = new_mode;
            return;
        }
    }
}

void Neostrip::show() {
    strip.Show();
}

// Pixelanzahl setzen
void Neostrip::set_number_of_pixels(int new_number_of_pixels) {
    if (new_number_of_pixels != number_of_pixels) {
        /*
        pixels.fill(pixels.Color(0, 0, 0));
        pixels.show();
        number_of_pixels = new_number_of_pixels;
        pixels.updateLength(number_of_pixels);
        pixels.fill(pixels.Color(0, 0, 0));
        pixels.show();
        */
    }
}

// Intervall setzen
void Neostrip::set_interval(int new_interval) {
    interval = new_interval;
}

// Muster setzen
void Neostrip::set_pattern(uint8_t* pattern, int pattern_length, int start, int repeat) {
    start = max(1, start) - 1;
    int end = (repeat == 0) ? number_of_pixels : min(repeat * pattern_length + start, number_of_pixels);
    Serial.println("Set_pattern");
    Serial.println(start);
    Serial.println(end);
    
    for (int i = start; i < end; i++) {
        uint8_t r, g, b;
        Serial.println(i);
        byteToRgb2222(pattern[i % pattern_length], &r, &g, &b); 
        strip.SetPixelColor(i, RgbColor(r, g, b));
    }
    last_tick_ms = millis();
}

// Rotation nach rechts
void Neostrip::rotate_right() {
    /*
    uint32_t last_pixel = strip.GetPixelColor(number_of_pixels - 1);
    for (int i = number_of_pixels - 1; i > 0; i--) {
        pixels.setPixelColor(i, pixels.getPixelColor(i - 1));
    }
    pixels.setPixelColor(0, last_pixel);
    pixels.show();
    */
}

// Rotation nach links
void Neostrip::rotate_left() {
    /*
    uint32_t first_pixel = pixels.getPixelColor(0);
    for (int i = 0; i < number_of_pixels - 1; i++) {
        pixels.setPixelColor(i, pixels.getPixelColor(i + 1));
    }
    pixels.setPixelColor(number_of_pixels - 1, first_pixel);
    pixels.show();
    */
}

// Verarbeitung von JSON-Daten
void Neostrip::process_input(const char* input_data) {
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, input_data);

    if (error) {
        Serial.print(F("Error parsing JSON: "));
        Serial.println(error.c_str());
        return;
    }
    Serial.println(F("Parsing JSON OK"));

    if (doc["pixels"].is<int>()) {
        Serial.println(F("set number of pixels"));
        set_number_of_pixels(doc["pixels"]);
    }
    if (doc["pattern"].is<JsonArray>()) {
        Serial.println(F("pattern detected"));
        if (doc["autoclear"] == 1) {
            Serial.println(F("autoclear detected"));
            strip.ClearTo(RgbColor(0, 0, 0));
        }
        JsonArray pattern = doc["pattern"];
        int pattern_length = pattern.size();
        uint8_t pattern_array[pattern_length];
        for (int i = 0; i < pattern_length; i++) {
            Serial.println((int)pattern[i]);
            pattern_array[i] = pattern[i];
        }
        set_pattern(pattern_array, pattern_length, doc["start"] | 0, doc["repeat"] | 0);
    }
    //if (doc["interval"].is<int>()) {
    //    set_interval(doc["interval"]);
    //}
    //if (doc["mode"].is<const char*>()) {
    //    set_mode(doc["mode"]);
    //}
}

void Neostrip::processing() {
    if (strcmp(mode, "off") == 0) {
        strip.ClearTo(RgbColor(0, 0, 0));
    } else if (strcmp(mode, "right") == 0) {
        rotate_right();
    } else if (strcmp(mode, "left") == 0) {
        rotate_left();
    }
}
