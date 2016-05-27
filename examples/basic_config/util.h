#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include <ArduinoJson.h>
#include "FS.h"
#include <functional>

typedef std::function<void(void)> void_callback_t;

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
        if (millis() - memo > 1000) {
            Serial.println("LONG PRESSED > 2000");
            callback();
            break;
        }
        yield();
      }
        yield();
    }
    yield();
}
