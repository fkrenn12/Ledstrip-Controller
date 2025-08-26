
#include "rgb.h"
#include <ArduinoJson.h>
#include <NeoPixelBus.h>
#include "main.h"
#include "strip.h"

const u8_t color_table[] = {0, 15, 30, 63}; 

// Konstruktor
Neostrip::Neostrip(u8_t neo_pin, u16_t interval, u16_t number_of_pixels)
    : neo_pin(neo_pin), number_of_pixels(number_of_pixels), interval(interval), animation_mode("off"), 
      update_mode("instant"), last_tick_ms(0), need_show(false),shadow_strip_dirty(false),brightness(2.0),
      color_format("rgb2222"), strip(number_of_pixels, neo_pin){
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

// Color Format setzen
void Neostrip::set_color_format(String new_format) {
    for (String valid_format : COLOR_FORMATS) {
        if (new_format == valid_format) {
            color_format = new_format;
            Serial.println("New color format is " + color_format);
            return;
        }
    }
    Serial.print("Set color format cancelled - leaved as" + color_format);
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
void Neostrip::set_number_of_pixels(u16_t new_number_of_pixels) {
        if (new_number_of_pixels > 0 && new_number_of_pixels <= MAX_PIXEL_PER_STRIP)
        number_of_pixels = new_number_of_pixels;

}

// Intervall setzen
void Neostrip::set_interval(u16_t new_interval) {
    if (interval > 0)
        interval = new_interval;
        last_tick_ms = millis();
}

void Neostrip::set_brightness(float new_brightness) {
    new_brightness = (new_brightness > 4) ? 4: new_brightness; // ensure it is between 0 and 4
    shadow_strip_dirty = (brightness != new_brightness) ? true : shadow_strip_dirty; // mark shadow strip as dirty if brightness changed
    brightness = new_brightness;
}

// Muster setzen
void Neostrip::set_pattern(JsonArray pattern, u16_t start, u16_t repeat, u16_t add) {
    u16_t pattern_length = pattern.size();
    start = max(1, (int)start) - 1;
    u16_t end = (repeat == 0) ? number_of_pixels : min(repeat * pattern_length + start, (int)number_of_pixels);
    // Serial.println("Set pattern -> First pixel " + String(start) + " Last pixel " + String(end));

    for (u16_t i = 0; i < number_of_pixels; i++) 
        shadow_strip[i] = (i >= start && i < end) ? pattern[(i-start) % pattern_length] : (add == 1) ? shadow_strip[i] : 0;
    shadow_strip_dirty = true;
    // Serial.println("Status of shadow strip dirty" + String( shadow_strip_dirty));
    
    if (update_mode == "instant") {
        transfer_shadow_into_strip_if_dirty();
    }
}

void Neostrip::transfer_shadow_into_strip_if_dirty() {
    Serial.println("Transfer shadow into strip if dirty " + String( shadow_strip_dirty));
    if (!shadow_strip_dirty) return;
    uint8_t r, g, b, individual_brightness, pixel;
    for (u16_t i = 0; i < number_of_pixels; i++) {
        pixel = shadow_strip[i];         
        // if brightness too low, clear pixel
        if ((brightness <= 0.06)) {
            strip.SetPixelColor(i, RgbColor(0, 0, 0));
            continue;
        }
        // Serial.println("Pixel " + String(i) + " is " + String(pixel));
        // Serial.println("Brightness is " + String(brightness));
        // byteToRgb2222(pixel, &r, &g, &b);
        individual_brightness = (pixel >> 6) & 0b11; 
        r = color_table[(pixel >> 4) & 0b11];
        g = color_table[(pixel >> 2) & 0b11];
        b = color_table[pixel & 0b11];
        r = individual_brightness ? r * (individual_brightness + 1) : (uint8_t)(r * brightness);
        g = individual_brightness ? g * (individual_brightness + 1) : (uint8_t)(g * brightness);
        b = individual_brightness ? b * (individual_brightness + 1) : (uint8_t)(b * brightness);
        // Serial.println(String(i) + " " + String(r) + " " + String(g) + " " + String(b));
        strip.SetPixelColor(i, RgbColor(r, g, b));
    }
    shadow_strip_dirty = false;
    need_show = true;
}

// Verarbeitung von JSON-Daten
void Neostrip::process_input(JsonDocument doc) {

    if (doc["update-mode"].is<String>()) {
        set_update_mode(doc["update-mode"]);
    }

    if (doc["interval"].is<int>()) {
        set_interval(doc["interval"]);
    }

    if (doc["animation-mode"].is<String>()) {
        set_animation_mode(doc["animation-mode"]);        
    }

    if (doc["color-format"].is<String>()) {
        set_color_format(doc["color-format"]);
    }

    if (doc["brightness"].is<float>()) {
        set_brightness(doc["brightness"]);
    }
    
    if (doc["pattern-add"].is<JsonArray>())
    {
        // Serial.println(F("pattern-add detected"));
        set_pattern(doc["pattern-add"], doc["first"] | 0, doc["repeat"] | 0, 1);
    }

    if (doc["pattern"].is<JsonArray>()) {
        // Serial.println(F("pattern detected"));
        set_pattern(doc["pattern"], doc["first"] | 0, doc["repeat"] | 0, 0);
    }
}

void Neostrip::animate() {
    if (animation_mode == "rotate-right") {
        strip.RotateRight(1);
        need_show = true;
    } else if (animation_mode == "rotate-left") {
        strip.RotateLeft(1);
        need_show = true;
    } else if (animation_mode == "shift-right") {
        strip.ShiftRight(1);
        need_show = true;
    } else if (animation_mode == "shift-left") {
        strip.ShiftLeft(1);
        need_show = true;
    }
}
