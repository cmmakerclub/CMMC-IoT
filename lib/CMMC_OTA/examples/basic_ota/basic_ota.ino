// Copyright Nat Weerawan 2015-2016
// MIT License

#include <ESP8266WiFi.h>
#include <CMMC_OTA.h>

CMMC_OTA ota;

void init_hardware()
{
  WiFi.disconnect(true);
  Serial.begin(115200);
  Serial.flush();
  Serial.println();
  Serial.println();
  Serial.println("being started...");
  delay(100);

// Serial.println("GOGOGO");
  WiFi.begin("ESPERT-002", "espertap");
  while (WiFi.status() != WL_CONNECTED) {
   Serial.println("connecting..");
   delay(100);
 }

  Serial.print("READY!! IP address: ");

  Serial.println(WiFi.localIP());
}

void setup()
{
  init_hardware();
  
  ota.on_progress([](unsigned int progress, unsigned int total){
      Serial.printf("_CALLBACK_ Progress: %u/%u\r\n", progress,  total);
  });

  ota.init();

  Serial.println("Chiang Mai Maker Club.");
}

void loop()
{
  ota.loop();
}
