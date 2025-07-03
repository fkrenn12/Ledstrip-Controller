#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <ArduinoJson.h>
#include "mainx.h"
#include "uart.h"
#include <map>
#include <string>

// std::map<int, std::string> pinMapping;
std::map<std::string, int> pinMapping;
// Werte hinzufügen (Key, Value)
// pinMapping["12"] = 13;

// Globale Variablen
Adafruit_NeoPixel rgb = Adafruit_NeoPixel(NUM_PIXELS, NEO_PIXEL_PIN_BUILTIN, NEO_GRB + NEO_KHZ800);
unsigned long last_tick_time = 0;
/*
#define NUM_STRIPS 6
void callback_idle() {
    // Iteriere durch das Array oder die Mapping der LED-Streifen
    for (int i = 0; i < NUM_STRIPS; i++) {
        Neostrip* led_strip = strip_mapping[i];
        
        // Überprüfe, ob die Zeitdifferenz größer als das Intervall ist
        if (millis() - led_strip->last_tick_ms > led_strip->interval) {
            led_strip->last_tick_ms = millis(); // Aktualisiere den letzten Tick
            led_strip->processing();           // Verarbeite den aktuellen Modus
        }
    }
}
*/

void idle() {
  digitalWrite(INDICATOR_LED_PIN, !digitalRead(INDICATOR_LED_PIN));
}
void_func_void callback_idle = idle;

void process_received_message(char* message) {
  Serial.write(message);
  Serial.write('\n');
  StaticJsonDocument<UART_BUFFER_SIZE> doc;
  // Parse den JSON-String
  DeserializationError error = deserializeJson(doc, message);
  if (error) {
    Serial.print("Fehler beim Parsen: ");
    Serial.println(error.c_str());
    return;
  }
  Serial.write("JSON-String erfolgreich geparst.\n");
  int pin = doc["pin"];
  JsonArray pattern = doc["pattern"];
  Serial.println(pin);
  /*
      if not message or not pin or pin not in NEO_PIXEL_PINS:
        return None

    print(f"Received: {message}")
    strip = strip_mapping.get(pin, None)
    if strip:
        strip.process_input(message)
  */ 
 
}

void setup() {
  Serial.begin(9600);  // monitor
  Serial1.begin(UART_BAUDRATE, SERIAL_8N1, UART_RX_PIN);
  pinMode(INDICATOR_LED_PIN, OUTPUT);
  digitalWrite(INDICATOR_LED_PIN, LOW); 
}

void loop() {
  char* line = read_uart_line_blocking(callback_idle,'\n');
  digitalWrite(INDICATOR_LED_PIN, HIGH);
  process_received_message(line);
  digitalWrite(INDICATOR_LED_PIN, LOW);
}
