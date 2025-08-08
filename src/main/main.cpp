#include <Arduino.h>
#include <NeoPixelBus.h>
#include <ArduinoJson.h>
#include "main.h"
#include "uart.h"
#include <map>
#include <string>
#include "strip.h"
#include "main.h"

void show_strips();
void create_strips();
// Globale Zuweisung der Streifen
const int NEO_PIXEL_PINS[NUM_STRIPS] = ARRAY_OF_ORDERED_STRIP_PINS;
Neostrip* strip_mapping[NUM_STRIPS];
int display_run_animation = DEFAULT_RUN_ANIMATION_AT_START;
int display_update_trigger = 0;

void create_strips() {
    Serial.println("Starting to create and initial strips");
    Serial.println("Create strips with max number of pixels");
    for (int i = 0; i < NUM_STRIPS; i++) {
        strip_mapping[i] = new Neostrip(NEO_PIXEL_PINS[i],DEFAULT_ANIMATION_INTERVAL_MS_AT_START, MAX_PIXEL_PER_STRIP);
    }
    return;
    // deleting and recreating does not work !!
    Serial.println("Destroying strips");
    for (int i = 0; i < NUM_STRIPS; i++) {
        Neostrip* strip =  strip_mapping[i];
        delete strip;
    }
    Serial.println("Recreate strips with default number of pixels");
    for (int i = 0; i < NUM_STRIPS; i++) {
        strip_mapping[i] = new Neostrip(NEO_PIXEL_PINS[i], DEFAULT_ANIMATION_INTERVAL_MS_AT_START,DEFAULT_NUMBER_OF_PIXELS_AT_START);
    }
    Serial.println("Finished creating and initial strips");
}

void show_strips() {
    for (int i = 0; i < NUM_STRIPS; i++) {
        Neostrip* strip = strip_mapping[i];
        strip->show();
    }
}

int index_of_strip_number(int strip) {
    if (strip < 1 || strip > NUM_STRIPS) return -1;
    return strip - 1;
}


void idle() {
    bool need_show_strips = false;
    u64_t currentMillis = millis();
    static u64_t last_tick_ms = 0;
    static bool previous_display_update_trigger = false;
    u16_t lowest_led_strip_interval = 2^16;
    bool display_update_now = (previous_display_update_trigger != display_update_trigger) && display_update_trigger; 
    previous_display_update_trigger = display_update_trigger;

    for (u8_t i = 0; i < NUM_STRIPS; i++) {
        Neostrip* led_strip = strip_mapping[i];
        // Überprüfe, ob die Aktualisierung sofort erfolgen soll
        if (led_strip->update_mode == "triggered" && display_update_now) led_strip->transfer_shadow_into_strip_if_dirty();
        lowest_led_strip_interval = (led_strip->interval < lowest_led_strip_interval) ? led_strip->interval : lowest_led_strip_interval;
        // Überprüfe, ob die Zeitdifferenz größer als das Intervall ist
        if (currentMillis - led_strip->last_tick_ms > led_strip->interval) {
            led_strip->last_tick_ms = display_run_animation ? currentMillis : 0;
            if (!display_run_animation) continue;
            led_strip->animate();
        }
        if (need_show_strips |= led_strip->need_show) {
            led_strip->need_show = false;
        }
    }
    if ((currentMillis - last_tick_ms > lowest_led_strip_interval) || need_show_strips)
    {
        last_tick_ms = currentMillis;
        digitalWrite(INDICATOR_LED_PIN, LOW);
        show_strips(); 
        digitalWrite(INDICATOR_LED_PIN, HIGH);
    }
}

void_func_void callback_idle = idle;


void process_strip(JsonDocument doc, int index)
{
    Serial.println("Index: " + String(index));
    if (index >= 0) {
        Serial.println("Strip is valid"); 
        Neostrip* strip = strip_mapping[index];
        int pixels = doc["pixels"] | -1;
        if ((pixels > 0) && (pixels != strip->strip.PixelCount())) {
            Serial.println("Deleting strip");
            strip->strip.ClearTo(RgbColor(0, 0, 0));
            delete strip;
            int interval = doc["interval"] | DEFAULT_ANIMATION_INTERVAL_MS_AT_START;
            Serial.println("Recreating strip");
            strip_mapping[index] = new Neostrip(NEO_PIXEL_PINS[index],interval,pixels);
        }
        strip_mapping[index]->process_input(doc);
    }
}

void process_received_message(char* message) {
    Serial.println(message);
    JsonDocument doc;
    // Parse den JSON-String
    DeserializationError error = deserializeJson(doc, message);
    if (error) {
        Serial.println("Error parsing JSON " + String(error.c_str()));
        return;
    }
    Serial.println(F("Parsing JSON OK"));

    if (doc["animation"].is<int>())
    {
        display_run_animation = doc["animation"];
        Serial.println("Animation is " + String(display_run_animation));
    }

    display_update_trigger = doc["update-trigger"] | 0;

    if (doc["strips"].is<JsonArray>()) {
        JsonArray strips = doc["strips"];
        for (u8_t i=0; i<strips.size();i++) process_strip(doc, (int)index_of_strip_number(strips[i]));
    }
    
    if (doc["strips"].is<int>()) process_strip(doc, (int)index_of_strip_number((int)doc["strips"]));
    if (doc["strip"].is<int>())  process_strip(doc, (int)index_of_strip_number((int)doc["strip"]));
}

void setup() {
  Serial.begin(9600);  // monitor
  Serial1.begin(UART_BAUDRATE, SERIAL_8N1, UART_RX_PIN);
  // Serial.println(heap_caps_get_free_size(MALLOC_CAP_INTERNAL));
  pinMode(INDICATOR_LED_PIN, OUTPUT);
  digitalWrite(INDICATOR_LED_PIN, HIGH); 
  create_strips();
  // Serial.println(heap_caps_get_free_size(MALLOC_CAP_INTERNAL));
}

void loop() {
  // endless main loop
  process_received_message(read_uart_line_blocking(callback_idle,'\n'));
}
