#include <Arduino.h>
// #include <Adafruit_NeoPixel.h>
#include <NeoPixelBus.h>
#include <ArduinoJson.h>
#include "mainx.h"
#include "uart.h"
#include <map>
#include <string>
#include "strip.h"

// Globale Zuweisung der Streifen
#define NUM_STRIPS 5
int NEO_PIXEL_PINS[NUM_STRIPS] = {2, 3, 4, 5, 6};
Neostrip* strip_mapping[NUM_STRIPS];

Neostrip strip2 = Neostrip(2);

void create_strips() {
    Serial.println("Starting to create and initial strips");
    for (int i = 0; i < NUM_STRIPS; i++) {
        Serial.println(NEO_PIXEL_PINS[i]);
        strip_mapping[i] = new Neostrip(NEO_PIXEL_PINS[i]);
    }
}

int index_of_pin(int pin) {
  for (size_t i = 0; i < NUM_STRIPS; i++) {
      if (NEO_PIXEL_PINS[i] == pin) return i; 
    }
    return -1; 
}

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
  // JsonArray pattern = doc["pattern"];
  Serial.println(pin);
  // int index = index_of_pin(pin);
  // Serial.print("Index: ");
  // Serial.println(index);
  //if (index >= 0) {
  //  Serial.println("Pin is valid");  
    // strip_mapping[index]->process_input(message);
  strip2.process_input(message);
  //}
 
  /*
  void loop() {
    // Beispiel: Verarbeitung der LED-Streifen aufrufen
    for (int i = 0; i < NUM_STRIPS; i++) {
        strip_mapping[i]->processing();
    }
}
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
  // create_strips();
}

void loop() {
  char* line = read_uart_line_blocking(callback_idle,'\n');
  digitalWrite(INDICATOR_LED_PIN, HIGH);
  process_received_message(line);
  digitalWrite(INDICATOR_LED_PIN, LOW);
}

// https://github.com/Makuna/NeoPixelBus/blob/master/examples/ESP32/NeoPixel_ESP32_LcdParallel/NeoPixel_ESP32_LcdParallel.ino

/*
//
// NeoPixel_ESP32_LcdParallel - 
// This sketch demonstrates the use of the LCD Parallel method allowing upto 8 or 16 hardware updated channels
// This example only works on the ESP32S3
// 
// The key part of the method name is Esp32LcdX8, 
//    E2p32 (platform specific method),
//    Lcd peripheral, 
//    X8 (8 parallel channel mode, x16 is also supported)
//
// In this example, it demonstrates different ColorFeatures, Method specification, and count per strip
// Note, the first instance of a NeoPixelBus will set the overall timing of all other instances
//
#include <NeoPixelBus.h>

// Demonstrating the use of the first four channels, but the method used allows for eight
NeoPixelBus<NeoBgrFeature, NeoEsp32LcdX8Ws2811Method> strip1(120, 15); // note: older WS2811 and longer strip
NeoPixelBus<NeoGrbFeature, NeoEsp32LcdX8Ws2812xMethod> strip2(100, 2); // note: modern WS2812 with letter like WS2812b
NeoPixelBus<NeoGrbFeature, NeoEsp32LcdX8Ws2812xInvertedMethod> strip3(100, 4); // note: inverted
NeoPixelBus<NeoGrbwFeature, NeoEsp32LcdX8Sk6812Method> strip4(50, 16); // note: RGBW and Sk6812 and smaller strip

void setup() {
    Serial.begin(115200);
    while (!Serial); // wait for serial attach

    Serial.println();
    Serial.println("Initializing...");
    Serial.flush();

    // must call begin on all the strips
    strip1.Begin();
    strip2.Begin();
    strip3.Begin();
    strip4.Begin();

    Serial.println();
    Serial.println("Running...");
}

void loop() {
    delay(1000);

    // draw on the strips
    strip1.SetPixelColor(0, RgbColor(255, 0, 0));      // red
    strip2.SetPixelColor(0, RgbColor(0, 127, 0));      // green
    strip3.SetPixelColor(0, RgbColor(0, 0, 53));       // blue
    strip4.SetPixelColor(0, RgbwColor(0, 0, 128, 255)); // white channel with a little blue

    // show them, 
    // only on the last show, no matter the order, will the data be sent
    strip1.Show();
    strip2.Show();
    strip3.Show();
    strip4.Show();
}
*/