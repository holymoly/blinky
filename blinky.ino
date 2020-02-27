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

// Controll WS2812b
#include <Adafruit_NeoPixel.h>

//#################
// LED Config
//#################
#include "Balls.h"
#define PIN D1                    // PIN the LED Stripe is connected to
#define NUMPIXELS 150             // Amount of LEDs
#define nBalls 3                  // number of balls for balls program
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
Balls balls[nBalls];


AsyncMqttClient mqttClient;
Ticker mqttReconnectTimer;

long triggerTime;

bool ledState = false;

bool mqttDebugActive = true;      //debug over mqtt

void setup() {
  Serial.begin(115200);
  Serial.println("Booting");
  
  //### LED Setup ###
  pinMode(PIN, OUTPUT);
  for(int i = 0; i <= nBalls-1; i++){
    balls[i].positionHoldPeriod = random(10, 50);
    balls[i].red = random(0, 150);
    balls[i].green = random(0, 150);
    balls[i].blue = random(0, 150);
    balls[i].startPosition = 1;
    balls[i].endPosition = NUMPIXELS;
    balls[i].actPosition = random(1, NUMPIXELS);
    balls[i].direction = random(0, 1);
    balls[i].speed = random(3, 10);
    balls[i].gravitation = 10;
  }
  //##################

  pinMode(2, OUTPUT);             // Initialize the LED_BUILTIN pin as an output
  digitalWrite(2, LOW);           // Turn the LED on by making the voltage LOW

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
  setLeds(); // updates the colors for the leds
  pixels.show(); // This sends the updated pixel color to the hardware.   
  
  if(triggerTime < millis()){
    digitalWrite(2, LOW);
    triggerTime = millis() + 1000;
    mqttDebug("döner",0);
  } else {
    digitalWrite(2, HIGH);
  }
}
