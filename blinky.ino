#include <FS.h>                   //this needs to be first, or it all crashes and burns...
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino

//needed for WiFi
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager

//needed for mqtt
#include <AsyncMqttClient.h>
#include <ESPAsyncTCP.h>
#include <tcp_axtls.h>
#include <async_config.h>
#include <AsyncPrinter.h>
#include <ESPAsyncTCPbuffer.h>
#include <SyncClient.h>
#include <Ticker.h>

// needed to upload sketches ovet the air
#include <ArduinoOTA.h>
#include <ArduinoJson.h>          //https://github.com/bblanchon/ArduinoJson

AsyncMqttClient mqttClient;
Ticker mqttReconnectTimer;

long triggerTime;

bool ledState = false;

bool mqttDebugActive = true; //debug over mqtt

void setup() {
  Serial.begin(115200);
  Serial.println("Booting");
  
  pinMode(2, OUTPUT);     // Initialize the LED_BUILTIN pin as an output
  digitalWrite(2, LOW);   // Turn the LED on by making the voltage LOW

  //clean FS, for testing
  //SPIFFS.format();

  if (SPIFFS.begin()) {
    Serial.println("mounted file system");
  } else {
    Serial.println("failed to mount FS");
  }

  // Start WiFi
  WiFIManagerInit();

  // Start mqtt
  mqttInit();
  connectToMqtt();

  // Prepare over the air updates
  otaInit();

  triggerTime = millis();
}

void loop() {
  ArduinoOTA.handle();
  if(triggerTime < millis()){
    digitalWrite(2, LOW);
    triggerTime = millis() + 1000;
    mqttDebug("döner",0);
  } else {
    digitalWrite(2, HIGH);
  }
}
