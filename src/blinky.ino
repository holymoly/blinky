#include <FS.h>                   //this needs to be first, or it all crashes and burns...

#ifdef ARDUINO_ARCH_ESP32
  #include <SPIFFS.h>
  #include <WebServer.h>
  #include <AsyncTCP.h>
  //needed for mqtt
  #include <AsyncMqttClient.h>
  #include <Ticker.h>
#elif defined(ARDUINO_ARCH_ESP8266) 
  #include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
  #include <ESP8266WebServer.h>
  #include <ESPAsyncTCP.h>

  //needed for mqtt
  #include <AsyncMqttClient.h>
  #include <tcp_axtls.h>
  #include <async_config.h>
  #include <AsyncPrinter.h>
  #include <ESPAsyncTCPbuffer.h>
  #include <SyncClient.h>
  #include <Ticker.h>
#endif


//needed for WiFi
#include <DNSServer.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager

// needed to upload sketches over the air
#include <ArduinoOTA.h>
#include <ArduinoJson.h>          //https://github.com/bblanchon/ArduinoJson

// Controll WS2812b
#include <Adafruit_NeoPixel.h>

//#################
// LED Config
//#################
#include "Balls.h"
#ifdef ARDUINO_ARCH_ESP32
  #define PIN 13
  #define FORMAT_SPIFFS_IF_FAILED true
#elif defined(ARDUINO_ARCH_ESP8266)
  #define PIN D1                    // PIN the LED Stripe is connected to
#endif                // PIN the LED Stripe is connected to
#define NUMPIXELS 27*4            // Amount of LEDs
#define nBalls 3                  // number of balls for balls program
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
Balls balls[nBalls];

AsyncMqttClient mqttClient;

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
    balls[i].endPosition = pixels.numPixels();
    balls[i].actPosition = random(1, pixels.numPixels());
    balls[i].direction = random(0, 1);
    balls[i].speed = random(3, 10);
    balls[i].gravitation = 10;
  }
  //##################

  pinMode(2, OUTPUT);             // Initialize the LED_BUILTIN pin as an output
  digitalWrite(2, LOW);           // Turn the LED on by making the voltage LOW

  //clean FS, for testing
  //SPIFFS.format();


#ifdef ARDUINO_ARCH_ESP32
  if (SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED)) {
#elif defined(ARDUINO_ARCH_ESP8266)
  if (SPIFFS.begin()) {
#endif
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
    mqttDebug("peter du lappen",0);
  } else {
    digitalWrite(2, HIGH);
  }
}
