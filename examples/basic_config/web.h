#include <Arduino.h>
#include <ESP8266WebServer.h>

ESP8266WebServer server(80);
StaticJsonBuffer<200> jsonBuffer;





void init_webserver() {
  server.on ("/", []() {
    static String responseHTML = F(""
    "<!doctype html>"
    "<html>"
    "  <head>"
    ""
    "<script type=\"text/javascript\">"
    "                                 "
    "</script>"
    "  </head>"
    "  <body>"
    "     <div>"
    "         <form id='save' name='save' action='save' method='POST'>                "
    "           <div>                                                                 "
    "             ssid: <input value='ESPERT-002' type='text' id='ssid'/>  "
    "           </div>                                                                "
    "           <div>                                                                 "
    "             pass: <input value='espertap' type='text' id='pass'/>    "
    "           </div>                                                                "
    "           <div>                                                                 "
    "             name: <input value='NatNat' type='text' id='name' />     "
    "           </div>                                                                "
    "           <div>                                                                 "
    "             mqtt: <input value='mqtt.espert.io' type='text' id='mqttHost' />     "
    "           </div>                                                                "
    "           <div>                                                                 "
    "             rest: <input value='rest.espert.io' type='text' id='restPath' />     "
    "           </div>                                                                "
    "           <div>                                                                 "
    "             update freq: <input value='10' type='text' id='updateFreq' />     "
    "           </div>                                                                "
    "           <div>                                                                 "
    "             <input type=submit id='submitButton' name='submitButton'            "
    "                         value='Submit'/>                                        "
    "           </div>                                                                "
    "         </form>"
    "     </div>"
    "  </body>"
    "</html>");
    server.send (200, "text/html", responseHTML.c_str() );
  });


  server.on("/save", []() {

    String ssid = server.arg("ssid");
    String pass = server.arg("pass");
    String myName = server.arg("name");
    String restPath = server.arg("restPath");
    String mqttHost = server.arg("mqttHost");

    replaceString(ssid);
    replaceString(pass);
    replaceString(myName);

    JsonObject& json = jsonBuffer.createObject();

    json["ssid"] = ssid;
    json["pass"] = pass;
    json["name"] = myName;
    json["restPath"] = restPath;
    json["mqttHost"] = mqttHost;

    bool config = saveConfig(json);

    String out = String(config) + String(".... BEING REBOOTED.");
    server.send(200, "text/plain", out.c_str());
    WiFi.disconnect();
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid.c_str(), pass.c_str());
    delay(1000);
    ESP.reset();

  });

  server.on ( "/millis", []() {
    char buff[100];
    String ms = String(millis());
    sprintf(buff, "{\"millis\": %s }", ms.c_str());
    server.send ( 200, "text/plain", buff );
  });
}
