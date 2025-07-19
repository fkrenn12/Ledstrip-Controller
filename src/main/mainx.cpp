#include <Arduino.h>
#include <NeoPixelBus.h>
#include <ArduinoJson.h>
#include "mainx.h"
#include "uart.h"
#include <map>
#include <string>
#include "strip.h"

// {"pin": 2, "autoclear": 1, "pattern": [240,204,195] ,"repeat": 1, "interval":100, "mode":"right","first":1, "last":144, "count":144}

void show_all_strips();
void create_strips();
// Globale Zuweisung der Streifen
#define NUM_STRIPS 8
#define MAX_PIXEL_PER_STRIP 1000
int NEO_PIXEL_PINS[NUM_STRIPS] = {2,3,4,5,6,7,44,9};
Neostrip* strip_mapping[NUM_STRIPS];

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

void show_all_strips() {
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

// Globale Variablen

void idle() {
    bool need_show_strip = false;
    u64_t currentMillis = millis();
    static u64_t last_tick_ms = 0;

    int lowest_led_strip_interval = 1000000;
    for (int i = 0; i < NUM_STRIPS; i++) {
        Neostrip* led_strip = strip_mapping[i];
        if (led_strip->interval < lowest_led_strip_interval) {
            lowest_led_strip_interval = led_strip->interval;
        }
        // Überprüfe, ob die Zeitdifferenz größer als das Intervall ist
        if (currentMillis- led_strip->last_tick_ms > led_strip->interval) {
            led_strip->last_tick_ms = currentMillis; // Aktualisiere den letzten Tick
            led_strip->processing();           // Verarbeite den aktuellen Modus
        }
        need_show_strip = need_show_strip || led_strip->need_show;
        led_strip->need_show = false;
    }

    // if (need_show_strip) //only on pattern update
    if ((currentMillis - last_tick_ms > lowest_led_strip_interval)   || need_show_strip)
    {
        last_tick_ms = currentMillis;
        digitalWrite(INDICATOR_LED_PIN, HIGH);
        show_all_strips(); 
        digitalWrite(INDICATOR_LED_PIN, LOW);
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
    Serial.println("Error parsing the message - " + String(error.c_str()));
    return;
  }
  Serial.println("JSON message parsed successfully.");
  int pin = -1;
  if (doc["pin"].is<int>()) pin = doc["pin"]; 
  Serial.println(pin);
  int index = index_of_pin(pin);
  Serial.println("Index: " + String(index));
  // Serial.println(index);
  if (index < 0) return;
  Serial.println("Pin "+String(pin)+" is valid"); 

  Neostrip* strip = strip_mapping[index];
  if (doc["pixels"].is<int>()) {
        int pixels = doc["pixels"];
        if (pixels != strip->strip.PixelCount()) {
        Serial.println("set number of pixels");
        delete strip;
        int interval = DEFAULT_INTERVAL_MS;
        if (doc["interval"].is<int>()) interval = doc["interval"]; 
        strip_mapping[index] = new Neostrip(NEO_PIXEL_PINS[index],interval,pixels);
    }
  }
  strip_mapping[index]->process_input(message);
}

void setup() {
  // malloc(strip_mapping, NUM_STRIPS * sizeof(Neostrip*));
  Serial.begin(9600);  // monitor
  Serial1.begin(UART_BAUDRATE, SERIAL_8N1, UART_RX_PIN);
  Serial.println(heap_caps_get_free_size(MALLOC_CAP_INTERNAL));
  pinMode(INDICATOR_LED_PIN, OUTPUT);
  digitalWrite(INDICATOR_LED_PIN, LOW); 
  create_strips();
  Serial.println(heap_caps_get_free_size(MALLOC_CAP_INTERNAL));
}

void loop() {
  char* line = read_uart_line_blocking(callback_idle,'\n');
  // digitalWrite(INDICATOR_LED_PIN, HIGH);
  process_received_message(line);
  // digitalWrite(INDICATOR_LED_PIN, LOW);
}

// https://github.com/Makuna/NeoPixelBus/blob/master/examples/ESP32/NeoPixel_ESP32_LcdParallel/NeoPixel_ESP32_LcdParallel.ino

// ##########################################################
//  Dynamisches erstellen und löschen von Streifen
// ##########################################################
/*
// declare your object as dynamic, a pointer to it, the *
// a good practice is to set it NULL
NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod>* strip = NULL;

void setup() {
...
// inside setup, allocate your default, or maybe you don't do this and just wait for outside influence
    PixelCountChanged(1); 
...
}

bool PixelCountChanged(uint16_t newCount) {
    if (strip != NULL) {  
       delete strip; // delete the previous dynamically created strip
    }
    strip = new NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod>(newCount, Pin); // and recreate with new count
    if (strip == NULL) {
        Serial.println("OUT OF MEMORY");
        return false;
    }
    strip->Begin();
    return true;
}

void loop() {
// other parts of your code, you set the colors, and show
    if (strip != NULL) { 
        // some arbitrary code
        for (uint16_t pixel = 0; pixel < strip->PixelCount(); pixel++) {
            strip->SetPixelColor(pixel, red);
        }
        strip->Show();
    }
}

*/