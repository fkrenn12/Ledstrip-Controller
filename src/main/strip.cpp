#include "strip.h"
#include "rgb.h"
#include <ArduinoJson.h>
#include <NeoPixelBus.h>


// Konstruktor
Neostrip::Neostrip(int neo_pin, int interval, int number_of_pixels)
    : neo_pin(neo_pin), number_of_pixels(number_of_pixels), interval(interval), mode("on"), last_tick_ms(0), need_show(false),
      strip(number_of_pixels, neo_pin){
    strip.Begin();
    strip.ClearTo(RgbColor(0, 0, 0));
}

// Modus setzen
void Neostrip::set_mode(String new_mode) {
    for (String valid_mode : MODES) {
        if (new_mode == valid_mode) {
            mode = new_mode;
            Serial.println("New mode is " + mode);
            return;
        }
    }
    Serial.print("Set mode cancelled - leaved as" + mode);
}

void Neostrip::show() {
    need_show=false;
    strip.Show();
}

// Pixelanzahl setzen
void Neostrip::set_number_of_pixels(int new_number_of_pixels) {
        number_of_pixels = new_number_of_pixels;

}

// Intervall setzen
void Neostrip::set_interval(int new_interval) {
    if (interval > 0)
        interval = new_interval;
}

// Muster setzen
void Neostrip::set_pattern(uint8_t* pattern, int pattern_length, int start, int repeat) {
    start = max(1, start) - 1;
    int end = (repeat == 0) ? number_of_pixels : min(repeat * pattern_length + start, number_of_pixels);
    Serial.println("Set pattern...");
    Serial.println("First pixel " + String(start));
    Serial.println("Last pixel " + String(end));

    
    for (int i = start; i < end; i++) {
        uint8_t r, g, b;
        byteToRgb2222(pattern[i % pattern_length], &r, &g, &b); 
        strip.SetPixelColor(i, RgbColor(r, g, b));
    }
    // https://github.com/Makuna/NeoPixelBus/wiki/NeoPixelBus-object-API
    // bool CanShow() ??
    // bool IsDirty() ??
    need_show = true;
}

// Rotation nach rechts
void Neostrip::rotate_right() {
    strip.RotateRight(1);
    need_show = true;
}

// Rotation nach links
void Neostrip::rotate_left() {
    strip.RotateLeft(1);
    need_show = true;
}

// Verarbeitung von JSON-Daten
void Neostrip::process_input(JsonDocument doc) {
    int autoclear = doc["autoclear"] | 1;
    if (doc["pattern"].is<JsonArray>()) {
        Serial.println(F("pattern detected"));
        if (autoclear == 1) strip.ClearTo(RgbColor(0, 0, 0));
        JsonArray pattern = doc["pattern"];
        int pattern_length = pattern.size();
        uint8_t pattern_array[pattern_length];
        for (int i = 0; i < pattern_length; i++) {
            pattern_array[i] = pattern[i];
        }
        set_pattern(pattern_array, pattern_length, doc["start"] | 0, doc["repeat"] | 0);
    }

    if (doc["interval"].is<int>()) {
        set_interval(doc["interval"]);
    }

    if (doc["mode"].is<String>()) {
        set_mode(doc["mode"]);        
    }
}

void Neostrip::processing() {
    Serial.print("Processing:" + String(neo_pin));
    Serial.println(" : " + mode);
    if (mode == "off") {
        strip.ClearTo(RgbColor(0, 0, 0));
    } else if (mode == "rotate-right") {
        rotate_right();
    } else if (mode == "rotate-left") {
        rotate_left();
    }
}
