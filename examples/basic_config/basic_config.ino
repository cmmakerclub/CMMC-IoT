#include <Arduino.h>
#include <EEPROM.h>
#include <MqttConnector.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <CMMC_OTA.h>
#include <CMMCEasy.h>
#include "FS.h"
#include <functional>

MqttConnector *mqtt;
CMMC_OTA ota;
CMMCEasy easy;

#define WEBSERVER_MODE 1
#define MQTT_MODE       2

uint8_t running_mode = MQTT_MODE;

// CMMC_Interval ti;

#define MQTT_HOST         "mqtt.espert.io"
#define MQTT_PORT         1883
#define MQTT_USERNAME     ""
#define MQTT_PASSWORD     ""
#define MQTT_CLIENT_ID    ""
#define MQTT_PREFIX       "/CMMC"
#define PUBLISH_EVERY     (20*1000)// every 10 seconds

/* DEVICE DATA & FREQUENCY */
String DEVICE_NAME = "IOT-STARTER-001";

/* WIFI INFO */
String WIFI_SSID      =  String("ESPERT-002");
String WIFI_PASSWORD  =  String("espertap");

String restPath =  String("cmmc.io");

#include "util.h"
#include "web.h"
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

void init_ap_sta_wifi() {
  WiFi.disconnect();
  WiFi.mode(WIFI_AP_STA);
  EEPROM.begin(512);
  delay(500);
  WiFi.softAP(getId().c_str());
}

void init_sta_wifi() {
  WiFi.disconnect();

  delay(200);
  loadConfig();
  WiFi.begin(WIFI_SSID.c_str(), WIFI_PASSWORD.c_str());
  bool ok = true;

  while(WiFi.status() != WL_CONNECTED) {
    Serial.printf ("Connecting to %s:%s\r\n", WIFI_SSID.c_str(), WIFI_PASSWORD.c_str());
    easy.blinker.blink(20*(5*ok), LED_BUILTIN);
    ok != ok;
    delay(500);
  }

  // easy.blinker.blink(1500, LED_BUILTIN);
  easy.blinker.detach();
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.print("WiFi Connected. => ");
  Serial.println(WiFi.localIP());


}

void init_fs() {
  Serial.println("Mounting FS...");
  if (!SPIFFS.begin()) {
    Serial.println("Failed to mount file system");
    return;
  }

  Dir root = SPIFFS.openDir("/");
  Serial.println("CMMC READING ROOT..");
  while (root.next()) {
    String fileName = root.fileName();
    File f = root.openFile("r");
    Serial.printf("%s: %d\r\n", fileName.c_str(), f.size());
  }

  Serial.println("FS mounted.");
}

void setup()
{
  init_hardware();
  pinMode(13, INPUT_PULLUP);
  pinMode(0, INPUT_PULLUP);

  easy.blinker.init(CMMC_Blink::TYPE_TICKER);
  easy.blinker.blink(20, LED_BUILTIN);

  delay(1500);

  auto web_server_mode_fn = []() {
    Serial.println("LONG PRESSED !!!!");
    running_mode = WEBSERVER_MODE;
  };

  long_press_check(13, web_server_mode_fn);
  long_press_check(0, web_server_mode_fn);

  init_fs();

  if (running_mode == MQTT_MODE) {
    easy.blinker.blink(500, LED_BUILTIN);
    init_sta_wifi();
    init_mqtt();
    init_ota();
  }
  else if (running_mode == WEBSERVER_MODE){
    easy.blinker.blink(300, LED_BUILTIN);
    init_ap_sta_wifi();
    delay(1000);
    init_webserver();
  }
}

void loop()
{
  if (running_mode == MQTT_MODE) {
    mqtt->loop();
    long_press_check(13, []() {
      Serial.println("REST FIRED!!");
      Serial.println("REST FIRED!!");
      Serial.println("REST FIRED!!");
      httpGet();
    });
  }
  else if(running_mode == WEBSERVER_MODE) {
    server.handleClient();
  }

  ota.loop();
}
