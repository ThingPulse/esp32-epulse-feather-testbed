
#include <ArduinoJson.h>
#include "Arduino.h"
#include <WiFi.h>

#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  5        /* Time ESP32 will go to sleep (in seconds) */

#define BAT_MON_PIN 35

#define R1 2.2f
#define R2 4.7f

uint16_t getBatteryVoltageRaw() {
    return analogRead(BAT_MON_PIN);
}

float getBatteryVoltage() {
  // VOut * (R1 + R2)/R2
  float rawVoltage = getBatteryVoltageRaw() * 3.3 / 4095.0;
  float batteryVoltage =  rawVoltage * (R1 + R2) / R2;
  return batteryVoltage;
}

void addTestMessage(JsonArray& data, String name, String value, String result) {
    JsonObject object = data.createNestedObject();
    object["name"] = name;
    object["value"] = value;
    object["result"] = result;
}

void testDevice() {
  float vBat = getBatteryVoltage();
  StaticJsonDocument<1024> doc;
  JsonArray data = doc.to<JsonArray>();
  addTestMessage(data, "Mac Address", String(WiFi.macAddress()), "OK" );
  addTestMessage(data, "Chip Model", String(ESP.getChipModel()), "OK" );
  addTestMessage(data, "Chip Revision", String(ESP.getChipRevision()), "OK");
  addTestMessage(data, "Available Cores", String(ESP.getChipCores()), ESP.getChipCores() == 2 ? "OK" : "NOK");
  addTestMessage(data, "Heap Size", String(ESP.getHeapSize() / 1024) + "kb", ESP.getHeapSize() > 100000 ? "OK" : "NOK");
  addTestMessage(data, "Free Heap", String(ESP.getFreeHeap() / 1024) + "kb", ESP.getFreeHeap() > 100000 ? "OK" : "NOK");
  addTestMessage(data, "PSRAM Size", String(ESP.getPsramSize() / 1024) + "kb", ESP.getPsramSize() == 4192139 ? "OK" : "NOK");
  addTestMessage(data, "Free PSRAM", String(ESP.getFreePsram() / 1024) + "kb", ESP.getFreePsram() == 4192139 ? "OK" : "NOK");
  addTestMessage(data, "VBat", String(vBat) + "V", vBat > 3.8 ? "OK" : "NOK");
  addTestMessage(data, "Build Date", String(__DATE__), "OK");
  addTestMessage(data, "Build Time", String(__TIME__), "OK");
  serializeJson(doc, Serial);
  Serial.println();
}


void sleepTest() {
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  Serial.println("Going to sleep now");
  delay(1000);
  Serial.flush(); 
  esp_deep_sleep_start();
}

void setup() { 
  Serial.begin(115200);
}

void loop() {
  testDevice();
  delay(1000);
}

