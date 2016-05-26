#include<MqttConnector.h>

PubSubClient::callback_t on_message_arrived =
[&](const MQTT::Publish & pub) -> void {
    if(pub.payload_string() == "10") {
      //  digitalWrite(LED_BUILTIN, LOW);
       digitalWrite(13, LOW);
       digitalWrite(12, LOW);
       state = "10";
       Serial.println("10");
    } else if(pub.payload_string() == "11") {
       digitalWrite(13, HIGH);
       digitalWrite(12, HIGH);
       state = "11";
       Serial.println("11");
    }
 };
