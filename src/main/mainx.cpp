#include <Arduino.h>
#include <NeoPixelBus.h>
#include <ArduinoJson.h>
#include "mainx.h"
#include "uart.h"
#include <map>
#include <string>
#include "strip.h"

void show_all_strips();
void create_strips();
// Globale Zuweisung der Streifen
#define NUM_STRIPS 8
// int NEO_PIXEL_PINS[NUM_STRIPS] = {2, 3, 4, 5, 6};
int NEO_PIXEL_PINS[NUM_STRIPS] = {2,3,4,5,6,7,8,9};
Neostrip* strip_mapping[NUM_STRIPS];

void create_strips() {
    Serial.println("Starting to create and initial strips");
    for (int i = 0; i < NUM_STRIPS; i++) {
        Serial.println(NEO_PIXEL_PINS[i]);
        strip_mapping[i] = new Neostrip(NEO_PIXEL_PINS[i]);
    }
}

void show_all_strips() {
    Serial.println("update all strips");
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

#define NUM_STRIPS 6
void idle() {
    // Iteriere durch das Array oder die Mapping der LED-Streifen
    for (int i = 0; i < NUM_STRIPS; i++) {
        Neostrip* led_strip = strip_mapping[i];
        
        // Überprüfe, ob die Zeitdifferenz größer als das Intervall ist
        if (millis() - led_strip->last_tick_ms > led_strip->interval) {
            led_strip->last_tick_ms = millis(); // Aktualisiere den letzten Tick
            led_strip->processing();           // Verarbeite den aktuellen Modus
        }
    }
    show_all_strips();
    Serial.print("*");
}


//void idle() {
//  digitalWrite(INDICATOR_LED_PIN, !digitalRead(INDICATOR_LED_PIN));
//}
void_func_void callback_idle = idle;

void process_received_message(char* message) {
  Serial.write(message);
  Serial.write('\n');
  JsonDocument doc;
  // Parse den JSON-String
  DeserializationError error = deserializeJson(doc, message);
  if (error) {
    Serial.print("Fehler beim Parsen: ");
    Serial.println(error.c_str());
    return;
  }
  Serial.write("JSON-String erfolgreich geparst.\n");
  int pin = doc["pin"];
  Serial.println(pin);
  int index = index_of_pin(pin);
  Serial.print("Index: ");
  Serial.println(index);
  if (index >= 0) {
    Serial.println("Pin is valid");  
    strip_mapping[index]->process_input(message);
  }
  show_all_strips();
}

void setup() {
  Serial.begin(9600);  // monitor
  Serial1.begin(UART_BAUDRATE, SERIAL_8N1, UART_RX_PIN);
  pinMode(INDICATOR_LED_PIN, OUTPUT);
  digitalWrite(INDICATOR_LED_PIN, LOW); 
  create_strips();
}

void loop() {
  char* line = read_uart_line_blocking(callback_idle,'\n');
  digitalWrite(INDICATOR_LED_PIN, HIGH);
  process_received_message(line);
  digitalWrite(INDICATOR_LED_PIN, LOW);
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