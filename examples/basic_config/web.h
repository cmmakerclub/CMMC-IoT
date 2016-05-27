#include <Arduino.h>
#include <ESP8266WebServer.h>

ESP8266WebServer server(80);


bool saveConfig(String ssid, String pass, String name) {
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();
  json["ssid"] = ssid;
  json["pass"] = pass;
  json["name"] = name;

  File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile) {
    Serial.println("Failed to open config file for writing");
    return false;
  }

  json.printTo(configFile);
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

  WIFI_SSID = String(ssid);
  WIFI_PASSWORD = String(pass);
  DEVICE_NAME = String(name);


  // Real world application would store these values in some variables for
  // later use.

  Serial.print("Loaded ssid: ");
  Serial.println(ssid);
  Serial.print("Loaded pass: ");
  Serial.println(pass);
  Serial.print("Loaded name: ");
  Serial.println(name);
  return true;
}



void init_webserver() {
  server.on ("/", []() {
    static String responseHTML = F(""
    "<!doctype html>"
    "<html>"
    "  <head>"
    "    <script src='https://ajax.googleapis.com/ajax/libs/angularjs/1.5.3/angular.min.js'></script>" ""
    ""
    "<script type=\"text/javascript\">"
    "   angular.module(\"NatApp\", [])"
    "     .controller(\"CMMCController\", function($scope, $http, $interval) { "
    "       var successCallback = function(response) {                 "
    "          console.log(\"success\", response.data.millis);         "
    "          $scope.millis = response.data.millis;                   "
    "       };                                                         "
    "       var errorCallback = function(response) {                   "
    "          console.log(\"error\", response)                        "
    "       };                                                         "
    "       var polling = function() {                                 "
    "         $http({                                                  "
    "           method: 'GET',                                         "
    "           url: '/millis'                                         "
    "         }).then(successCallback, errorCallback);                 "
    "       };                                                         "
    "                                                                  "
    "       $interval(polling, 500);                                   "
    "       console.log($scope);                                       "
    "   });"
    "</script>"
    "  </head>"
    "  <body ng-app=\"NatApp\">"
    "  <h1>Hello World!</h1><p>This is an esp8266 webpage example."
    "  that you can embeded angular application inside! </p>"
    "  <h1>ESP8266 WebServer + AngularJS</h1>"
    "   <div ng-controller=\"CMMCController\">"
    "      <label>Input your name:</label>"
    "      <input type='text' ng-model='yourName' placeholder='Enter a name here'>"
    "      <hr>"
    "      <h1>[{{millis}}] Hello, {{yourName}}</h1>"
    "      <img src=\"https://www.cmmakerclub.com/wp-content/uploads/2014/07/logo1.png\" />"
    "    </div>"
    "  </body>"
    "</html>");
    server.send (200, "text/html", responseHTML.c_str() );
  });

  server.on("/save", []() {

    String ssid = server.arg("ssid");
    String pass = server.arg("pass");
    String myName = server.arg("myName");
    ssid.replace("+", " ");
    pass.replace("%40", "@");
    bool config = saveConfig(ssid, pass, myName);

    String out = String(config) + String(".... BEING REBOOTED."); 
    server.send(200, "text/plain", out.c_str());
    ESP.reset();

  });

  server.on ( "/millis", []() {
    char buff[100];
    String ms = String(millis());
    sprintf(buff, "{\"millis\": %s }", ms.c_str());
    server.send ( 200, "text/plain", buff );
  });
}
