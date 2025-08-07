#include <Arduino.h>
#include <NeoPixelBus.h>
#include <ArduinoJson.h>
#include "mainx.h"
#include "uart.h"
#include <map>
#include <string>
#include "strip.h"

// {"pin": 2, "autoclear": 1, "pattern": [240,204,195] ,"repeat": 1, "interval":100, "mode":"right","first":1, "last":144, "count":144}
// {"animation":"off", "show":"off", "pin": 3, "pixels":100, "autoclear":1, "pattern": [240, 200],  "repeat": 1, "interval":100, "mode":"rotate-right","first":1, "last":144, "count":144}

void show_strips();
void create_strips();
// Globale Zuweisung der Streifen
#define NUM_STRIPS 8
#define MAX_PIXEL_PER_STRIP 1000
int NEO_PIXEL_PINS[NUM_STRIPS] = {2,3,4,5,6,43,44,7};
Neostrip* strip_mapping[NUM_STRIPS];
int display_run_animation = 0;
int display_update_trigger = 0;

void create_strips() {
    Serial.println("Starting to create and initial strips");
    Serial.println("Create strips with max number of pixels");
    for (int i = 0; i < NUM_STRIPS; i++) {
        strip_mapping[i] = new Neostrip(NEO_PIXEL_PINS[i],DEFAULT_INTERVAL_MS, MAX_PIXEL_PER_STRIP);
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
        strip_mapping[i] = new Neostrip(NEO_PIXEL_PINS[i], DEFAULT_INTERVAL_MS,DEFAULT_NUMBER_OF_PIXELS);
    }
    Serial.println("Finished creating and initial strips");
}

void show_strips() {
    // Serial.println("update all strips");
    for (int i = 0; i < NUM_STRIPS; i++) {
        Neostrip* strip = strip_mapping[i];
        strip->show();
    }
}

int index_of_pin(int pin) {
  for (size_t i = 0; i < NUM_STRIPS; i++) {
      if (NEO_PIXEL_PINS[i] == pin) return i; 
  }
  return -1; 
}

void idle() {
    bool need_show_strips = false;
    u64_t currentMillis = millis();
    static u64_t last_tick_ms = 0;
    static bool previous_display_update_trigger = false;
    int lowest_led_strip_interval = 1000000;
    bool display_update_now = (previous_display_update_trigger != display_update_trigger) && display_update_trigger; 
    previous_display_update_trigger = display_update_trigger;

    for (int i = 0; i < NUM_STRIPS; i++) {
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

        /*
        if ((need_show_strips |= led_strip->need_show) && display_update_immediately) {
            led_strip->need_show = false;
        }
            */
        if (need_show_strips |= led_strip->need_show) {
            led_strip->need_show = false;
        }
    }


    // if (display_update_immediately && ((currentMillis - last_tick_ms > lowest_led_strip_interval) || need_show_strips))
    if ((currentMillis - last_tick_ms > lowest_led_strip_interval) || need_show_strips)
    {
        last_tick_ms = currentMillis;
        digitalWrite(INDICATOR_LED_PIN, LOW);
        show_strips(); 
        digitalWrite(INDICATOR_LED_PIN, HIGH);
    }
}


//void idle() {
//  digitalWrite(INDICATOR_LED_PIN, !digitalRead(INDICATOR_LED_PIN));
//}
void_func_void callback_idle = idle;

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

  int index = index_of_pin(doc["pin"] | -1);
  Serial.println("Index: " + String(index));

  if (index >= 0) {
    Serial.println("Pin is valid"); 
    Neostrip* strip = strip_mapping[index];
    int pixels = doc["pixels"] | -1;
    if ((pixels > 0) && (pixels != strip->strip.PixelCount())) {
        Serial.println("Deleting strip");
        delete strip;
        int interval = doc["interval"] | DEFAULT_INTERVAL_MS;
        Serial.println("Recreating strip");
        strip_mapping[index] = new Neostrip(NEO_PIXEL_PINS[index],interval,pixels);
    }
    strip_mapping[index]->process_input(doc);
}
}

void setup() {
  // malloc(strip_mapping, NUM_STRIPS * sizeof(Neostrip*));
  Serial.begin(9600);  // monitor
  Serial1.begin(UART_BAUDRATE, SERIAL_8N1, UART_RX_PIN);
  Serial.println(heap_caps_get_free_size(MALLOC_CAP_INTERNAL));
  pinMode(INDICATOR_LED_PIN, OUTPUT);
  digitalWrite(INDICATOR_LED_PIN, HIGH); 
  create_strips();
  Serial.println(heap_caps_get_free_size(MALLOC_CAP_INTERNAL));
}

void loop() {
  // endless main loop
  char* line = read_uart_line_blocking(callback_idle,'\n');
  process_received_message(line);
}
