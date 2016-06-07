#include <Arduino.h>
#include <ESP8266HTTPClient.h>

#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include <ArduinoJson.h>
#include "FS.h"
#include <functional>
#include "ESPert_JSON.hpp"

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
  ESPert_JSON *jsonParser;
  jsonParser = new ESPert_JSON;


  // jsonParser.init(buf.get());

  // JsonObject& json = jsonBuffer.parseObject(buf.get());

  if (!jsonParser->init(String(buf.get()))) {
    Serial.println("Failed to parse config file");
    return false;
  }


  // const char* ssid = json["ssid"];
  // const char* pass = json["pass"];
  // const char* name = json["name"];
  //
  WIFI_SSID     = jsonParser->get("ssid");
  WIFI_PASSWORD = jsonParser->get("pass");
  DEVICE_NAME   = jsonParser->get("name");
  restPath      = jsonParser->get("restPath");

  jsonParser->dumpSerial();

  Serial.printf("Loaded ssid: %s\r\n", WIFI_SSID.c_str());
  Serial.printf("Loaded pass: %s\r\n", WIFI_PASSWORD.c_str());
  Serial.printf("Loaded name: %s\r\n", DEVICE_NAME.c_str());
  Serial.printf("Loaded restPath: %s\r\n", restPath.c_str());
  delete jsonParser;
  return true;
}


void httpGet() {
  HTTPClient http;

  Serial.print("[HTTP] begin...\n");

  http.begin(restPath); //HTTP
  Serial.print("[HTTP] GET...\n");
  // start connection and send HTTP header
  int httpCode = http.GET();

  // httpCode will be negative on error
  if(httpCode > 0) {
    // HTTP header has been send and Server response header has been handled
    Serial.printf("[HTTP] GET... code: %d\n", httpCode);
    // file found at server
    if(httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        Serial.println(payload);
    }
  } else {
    Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();
}
