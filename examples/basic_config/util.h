#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include <ArduinoJson.h>
#include "FS.h"
#include <functional>

typedef std::function<void(void)> void_callback_t;

void replaceString(String input) {
  input.replace("+", " ");
  input.replace("%40", "@");
}

String getId() {
  char textID[16] = {'\0'};
  sprintf(textID, "ESPresso-%lu", ESP.getChipId());
  return String(textID);
}

String getAPIP() {
  IPAddress ip =  WiFi.softAPIP();
  char textID[32] = {'\0'};
  sprintf(textID, "%i.%i.%i.%i", ip[0], ip[1], ip[2], ip[3]);
  return String(textID);
}

// EEPROM
String readEEPROM(int index, int length) {
  String text = "";
  char ch = 1;

  for (int i = index; (i < (index + length)) && ch; ++i) {
    if (ch = EEPROM.read(i)) {
      text.concat(ch);
    }
  }

  return text;
}

int writeEEPROM(int index, String text) {
  for (int i = index; i < text.length() + index; ++i) {
    EEPROM.write(i, text[i - index]);
  }

  EEPROM.write(index + text.length(), 0);
  EEPROM.commit();

  return text.length() + 1;
}

void long_press_check(uint8_t pin, void_callback_t callback) {
    long memo = millis();
    if (digitalRead(pin) == LOW) {
      while (digitalRead(pin) == LOW) {
        if (millis() - memo > 1) {
            Serial.println("LONG PRESSED > 1ms");
            callback();
            break;
        }
        yield();
      }
        yield();
    }
    yield();
}


bool saveConfig(JsonObject& json) {
  File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile) {
    Serial.println("Failed to open config file for writing");
    return false;
  }

  json.printTo(Serial);
  json.printTo(configFile);
  Serial.println();

  return true;
}



bool loadConfig() {
  File configFile = SPIFFS.open("/config.json", "r");
  if (!configFile) {
    Serial.println("Failed to open config file");
    return false;
  }

  size_t size = configFile.size();
  if (size > 1024) {
    Serial.println("Config file size is too large");
    return false;
  }

  // Allocate a buffer to store contents of the file.
  std::unique_ptr<char[]> buf(new char[size]);

  // We don't use String here because ArduinoJson library requires the input
  // buffer to be mutable. If you don't use ArduinoJson, you may as well
  // use configFile.readString instead.
  configFile.readBytes(buf.get(), size);

  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& json = jsonBuffer.parseObject(buf.get());

  if (!json.success()) {
    Serial.println("Failed to parse config file");
    return false;
  }

  const char* ssid = json["ssid"];
  const char* pass = json["pass"];
  const char* name = json["name"];

  WIFI_SSID     = String(ssid);
  WIFI_PASSWORD = String(pass);
  DEVICE_NAME   = String(name);


  Serial.print("Loaded ssid: ");
  Serial.println(ssid);
  Serial.print("Loaded pass: ");
  Serial.println(pass);
  Serial.print("Loaded name: ");
  Serial.println(name);
  return true;
}
