  // rgb.setPixelColor(1, rgb.Color(COLOR_ACTIVE));
  // rgb.show();
#include <Adafruit_NeoPixel.h>
#include <ArduinoJson.h>
#include "rgb.h"
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

public:
    // Konstruktor
    Neostrip(int neo_pin, int interval = DEFAULT_INTERVAL_MS, int number_of_pixels = DEFAULT_NUMBER_OF_PIXELS)
      : neo_pin(neo_pin), number_of_pixels(number_of_pixels), interval(interval), mode("off"), last_tick_ms(0),
        pixels(number_of_pixels, neo_pin, NEO_GRB + NEO_KHZ800) 
    {
        pixels.begin();
        pixels.fill(pixels.Color(0, 0, 0));
        pixels.show();
    }

    // Modus setzen
    void set_mode(const char* new_mode) {
        for (const char* valid_mode : MODES) {
            if (strcmp(new_mode, valid_mode) == 0) {
                mode = new_mode;
                return;
            }
        }
    }

    // Pixelanzahl setzen
    void set_number_of_pixels(int new_number_of_pixels) {
        if (new_number_of_pixels != number_of_pixels) {
            pixels.fill(pixels.Color(0, 0, 0));
            pixels.show();
            number_of_pixels = new_number_of_pixels;
            pixels.updateLength(number_of_pixels);
        }
    }

    // Intervall setzen
    void set_interval(int new_interval) {
        interval = new_interval;
    }

    // Muster setzen
    void set_pattern(uint8_t* pattern, int pattern_length, int start = 0, int repeat = 0) {
        start = max(1, start) - 1;
        int end = (repeat == 0) ? number_of_pixels : min(repeat * pattern_length + start, number_of_pixels);

        for (int i = start; i < end; i++) {
            uint8_t r, g, b;
            byteToRgb2222(pattern[i % pattern_length], &r, &g, &b);  // Annahme: Hilfsfunktion existiert
            pixels.setPixelColor(i, pixels.Color(r, g, b));
        }

        if (strcmp(mode, MODES[0]) == 0) {
            set_mode(MODES[1]);
        }
        pixels.show();
        last_tick_ms = millis();
    }

    // Rotation nach rechts
    void rotate_right() {
        uint32_t last_pixel = pixels.getPixelColor(number_of_pixels - 1);
        for (int i = number_of_pixels - 1; i > 0; i--) {
            pixels.setPixelColor(i, pixels.getPixelColor(i - 1));
        }
        pixels.setPixelColor(0, last_pixel);
        pixels.show();
    }

    // Rotation nach links
    void rotate_left() {
        uint32_t first_pixel = pixels.getPixelColor(0);
        for (int i = 0; i < number_of_pixels - 1; i++) {
            pixels.setPixelColor(i, pixels.getPixelColor(i + 1));
        }
        pixels.setPixelColor(number_of_pixels - 1, first_pixel);
        pixels.show();
    }

    // Verarbeitet Eingaben
    void process_input(const char* input_data) {
        StaticJsonDocument<256> doc;
        DeserializationError error = deserializeJson(doc, input_data);

        if (error) {
            Serial.print(F("Error parsing JSON: "));
            Serial.println(error.c_str());
            return;
        }

        if (doc.containsKey("pixels")) {
            set_number_of_pixels(doc["pixels"]);
        }
        if (doc.containsKey("pattern")) {
            if (doc["autoclear"] == 1) {
                pixels.fill(pixels.Color(0, 0, 0));
            }
            JsonArray pattern = doc["pattern"];
            int pattern_length = pattern.size();
            uint8_t pattern_array[pattern_length];
            for (int i = 0; i < pattern_length; i++) {
                pattern_array[i] = pattern[i];
            }
            set_pattern(pattern_array, pattern_length, doc["start"] | 0, doc["repeat"] | 0);
        }
        if (doc.containsKey("interval")) {
            set_interval(doc["interval"]);
        }
        if (doc.containsKey("mode")) {
            set_mode(doc["mode"]);
        }
    }

    // Verarbeitet den aktuellen Modus
    void processing() {
        if (strcmp(mode, "off") == 0) {
            pixels.fill(pixels.Color(0, 0, 0));
        } else if (strcmp(mode, "right") == 0) {
            rotate_right();
        } else if (strcmp(mode, "left") == 0) {
            rotate_left();
        }
        pixels.show();
    }
};

/*
// Globale Zuweisung der Streifen
#define NUM_STRIPS 3
int NEO_PIXEL_PINS[NUM_STRIPS] = {6, 7, 8};
Neostrip* strip_mapping[NUM_STRIPS];

void setup() {
    Serial.begin(9600);
    for (int i = 0; i < NUM_STRIPS; i++) {
        strip_mapping[i] = new Neostrip(NEO_PIXEL_PINS[i]);
    }
}

void loop() {
    for (int i = 0; i < NUM_STRIPS; i++) {
        strip_mapping[i]->processing();
    }
    delay(100);
}
*/