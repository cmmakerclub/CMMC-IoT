#include <Arduino.h>
#include <MqttConnector.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <CMMC_OTA.h>

MqttConnector *mqtt;
CMMC_OTA ota;

#define MQTT_PORT         1883
#define MQTT_USERNAME     ""
#define MQTT_PASSWORD     ""
#define MQTT_CLIENT_ID    ""
#define MQTT_PREFIX       "/CMMC"
#define PUBLISH_EVERY     (5*1000)// every 10 seconds

/* DEVICE DATA & FREQUENCY */
#define DEVICE_NAME       "NAT-BASIC-CMMC-000"

/* WIFI INFO */
#ifndef WIFI_SSID
  #define WIFI_SSID        "Nat"
  #define WIFI_PASSWORD    "123456789"
#endif

#include "_publish.h"
#include "_receive.h"
#include "init_mqtt.h"


void init_hardware()
{
  Serial.begin(115200);
  delay(10);
  Serial.println();
  Serial.println("Serial port initialized.");
  pinMode(LED_BUILTIN, OUTPUT);

}

void init_ota() {
  Serial.println("[initialize] OTA");

  ota.on_progress([](unsigned int progress, unsigned int total){
      Serial.printf("_CALLBACK_ Progress: %u/%u\r\n", progress,  total);
  });


  ota.init();
}

void init_wifi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while(WiFi.status() != WL_CONNECTED) {
    Serial.printf ("Connecting to %s:%s\r\n", WIFI_SSID, WIFI_PASSWORD);
    delay(300);
  }

  Serial.print("WiFi Connected. => ");
  Serial.println(WiFi.localIP());
}

void setup()
{
  init_hardware();
  init_wifi();
  init_ota();
  init_mqtt();
}

void loop()
{
  mqtt->loop();
  ota.loop();
}
