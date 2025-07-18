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
void Neostrip::set_mode(const char* new_mode) {
    for (const char* valid_mode : MODES) {
        Serial.print(valid_mode);
        Serial.println(new_mode);
        if (strcmp(new_mode, valid_mode) == 0) {
            strcpy(mode, new_mode);
            Serial.println("set-mode done");
            Serial.print("new mode is ");
            Serial.println(mode);
            return;
        }
    }
    Serial.println("set-mode cancelled");
    Serial.print("mode leaved as ");
    Serial.println(mode);
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
    Serial.println("Set_pattern");
    Serial.println(start);
    Serial.println(end);
    
    for (int i = start; i < end; i++) {
        uint8_t r, g, b;
        // Serial.println(i);
        byteToRgb2222(pattern[i % pattern_length], &r, &g, &b); 
        strip.SetPixelColor(i, RgbColor(r, g, b));
    }
    // https://github.com/Makuna/NeoPixelBus/wiki/NeoPixelBus-object-API
    // bool CanShow() ??
    // bool IsDirty() ??

    need_show = true;
    //last_tick_ms = millis();
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
void Neostrip::process_input(const char* input_data) {
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, input_data);

    if (error) {
        Serial.print(F("Error parsing JSON: "));
        Serial.println(error.c_str());
        return;
    }
    Serial.println(F("Parsing JSON OK"));
    int autoclear = 1;
    if (doc["autoclear"].is<int>()) autoclear = doc["autoclear"]; 
    if (doc["pattern"].is<JsonArray>()) {
        Serial.println(F("pattern detected"));
        if (autoclear == 1) strip.ClearTo(RgbColor(0, 0, 0));
        JsonArray pattern = doc["pattern"];
        int pattern_length = pattern.size();
        uint8_t pattern_array[pattern_length];
        for (int i = 0; i < pattern_length; i++) {
            Serial.println((int)pattern[i]);
            pattern_array[i] = pattern[i];
        }
        set_pattern(pattern_array, pattern_length, doc["start"] | 0, doc["repeat"] | 0);
    }
    if (doc["interval"].is<int>()) {
        set_interval(doc["interval"]);
    }

    if (doc["mode"].is<const char*>()) {
        set_mode(doc["mode"]);
        // strcpy(mode, doc["mode"]);
    }
}

void Neostrip::processing() {
    Serial.print("Processing:");
    Serial.print(neo_pin);
    Serial.print(":");
    Serial.println(mode);
    if (strcmp(mode, "off") == 0) {
        strip.ClearTo(RgbColor(0, 0, 0));
    } else if (strcmp(mode, "rotate-right") == 0) {
        rotate_right();
    } else if (strcmp(mode, "rotate-left") == 0) {
        rotate_left();
    }
}
