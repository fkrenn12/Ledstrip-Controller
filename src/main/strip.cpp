#include "strip.h"
#include "rgb.h"
#include <ArduinoJson.h>
#include <NeoPixelBus.h>


// Konstruktor
Neostrip::Neostrip(int neo_pin, int interval, int number_of_pixels)
    : neo_pin(neo_pin), number_of_pixels(number_of_pixels), interval(interval), animation_mode("on"), 
      update_mode("immediately"), last_tick_ms(0), need_show(false),shadow_strip_dirty(false),
      strip(number_of_pixels, neo_pin){
    strip.Begin();
    strip.ClearTo(RgbColor(0, 0, 0));
}

// Animation Modus setzen
void Neostrip::set_animation_mode(String new_mode) {
    for (String valid_mode : ANIMATION_MODES) {
        if (new_mode == valid_mode) {
            animation_mode = new_mode;
            Serial.println("New animation mode is " + animation_mode);
            return;
        }
    }
    Serial.print("Set animation mode cancelled - leaved as" + animation_mode);
}

// Update Modus setzen
void Neostrip::set_update_mode(String new_mode) {
    for (String valid_mode : UPDATE_MODES) {
        if (new_mode == valid_mode) {
            update_mode = new_mode;
            Serial.println("New Update mode is " + update_mode);
            return;
        }
    }
    Serial.print("Set update mode cancelled - leaved as" + update_mode);
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
    Serial.println("Set pattern -> First pixel " + String(start) + " Last pixel " + String(end));

    for (int i = 0; i < number_of_pixels; i++) {
        shadow_strip[i] = (i >= start && i < end) ? pattern[i % pattern_length] : 0;
        // Serial.println("Shadow strip " + String(i) + " " + String(shadow_strip[i]));
    }
    shadow_strip_dirty = true;
    Serial.println("Status of shadow strip dirty" + String( shadow_strip_dirty));
    
    if (update_mode == "immediately") {
        transfer_shadow_into_strip_if_dirty();
    }

    /*
    for (int i = 0; i < number_of_pixels; i++) {
        if (i >= start && i < end) {
            shadow_strip[i] = pattern[i % pattern_length];
            shadow_strip_dirty = true;
        } else {
            shadow_strip[i] = 0;
        }
    }
    */
    /*for (int i = start; i < end; i++) {
        uint8_t r, g, b;
        byteToRgb2222(pattern[i % pattern_length], &r, &g, &b); 
        strip.SetPixelColor(i, RgbColor(r, g, b));
        shadow_strip[i] = pattern[i % pattern_length];
        shadow_strip_dirty = true;
    }
        */
    // https://github.com/Makuna/NeoPixelBus/wiki/NeoPixelBus-object-API
    // bool CanShow() ??
    // bool IsDirty() ??
    // need_show = true;
}

void Neostrip::transfer_shadow_into_strip_if_dirty() {
    Serial.println("Transfer shadow into strip if dirty " + String( shadow_strip_dirty));
    if (!shadow_strip_dirty) return;
    for (int i = 0; i < number_of_pixels; i++) {
        uint8_t r, g, b;
        byteToRgb2222(shadow_strip[i], &r, &g, &b); 
        // Serial.println(String(i) + " " + String(r) + " " + String(g) + " " + String(b));
        strip.SetPixelColor(i, RgbColor(r, g, b));
    }
    shadow_strip_dirty = false;
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

    if (doc["update"].is<String>()) {
        set_update_mode(doc["update"]);
    }

    if (doc["interval"].is<int>()) {
        set_interval(doc["interval"]);
    }

    if (doc["mode"].is<String>()) {
        set_animation_mode(doc["mode"]);        
    }

    if (doc["pattern"].is<JsonArray>()) {
        Serial.println(F("pattern detected"));
        if (autoclear == 1 && update_mode == "immediately") strip.ClearTo(RgbColor(0, 0, 0));
        JsonArray pattern = doc["pattern"];
        int pattern_length = pattern.size();
        uint8_t pattern_array[pattern_length];
        for (int i = 0; i < pattern_length; i++) {
            pattern_array[i] = pattern[i];
        }
        set_pattern(pattern_array, pattern_length, doc["start"] | 0, doc["repeat"] | 0);
    }

    
}

void Neostrip::animate() {
    // Serial.print("Animate:" + String(neo_pin));
    // Serial.println(" : " + animation_mode);
    if (animation_mode == "rotate-right") {
        rotate_right();
    } else if (animation_mode == "rotate-left") {
        rotate_left();
    }
}
