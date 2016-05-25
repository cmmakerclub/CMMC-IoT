#include <Arduino.h>
#include <MqttConnector.h>
#include <WiFiConnector.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>

MqttConnector *mqtt;

#define MQTT_HOST         "mqtt.espert.io"
#define MQTT_PORT         1883
#define MQTT_USERNAME     ""
#define MQTT_PASSWORD     ""
#define MQTT_CLIENT_ID    ""
#define MQTT_PREFIX       "/CMMC"
#define PUBLISH_EVERY     (10*1000)// every 10 seconds

/* DEVICE DATA & FREQUENCY */
#define DEVICE_NAME       "CNX-CMMC-003"

/* WIFI INFO */
#ifndef WIFI_SSID
  #define WIFI_SSID        "Nat"
  #define WIFI_PASSWORD    "123456789"
#endif


WiFiConnector wifi(WIFI_SSID, WIFI_PASSWORD);

#include "_publish.h"
#include "_receive.h"
#include "init_mqtt.h"

void init_hardware()
{
  Serial.begin(115200);
  delay(10);
  Serial.println();
  Serial.println("BEGIN");
}

void init_wifi() {
  wifi.init();

  wifi.on_connected([&](const void* message)
  {
    Serial.print("WIFI CONNECTED WITH IP: ");
    Serial.println(WiFi.localIP());
  });

  wifi.on_connecting([&](const void* message)
  {
    Serial.print("Connecting to ");
    Serial.println(wifi.get("ssid") + ", " + wifi.get("password"));
    delay(200);
  });

  wifi.connect();
}

void setup()
{
  init_hardware();
  init_wifi();
  init_mqtt();
}

void loop()
{
  wifi.loop();
  if (wifi.connected()) {
    mqtt->loop();
  }
  else {
    Serial.println("WiFi Disconnected..");
    delay(400);
  }
}
