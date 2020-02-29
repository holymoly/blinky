/*
 * If mqtt packe was type command
 */
void parseMqttCommand(const JsonDocument& message){
  serializeJson(message, Serial);

  // check if WiFI should be reseted
  if(strcmp(message["cmd"].as<char *>(), "wifiReset") == 0){
    Serial.println("Received message WiFi reset");
    mqttCmdWiFiReset();
  }

  // check if debug should be switched on/off
  if(strcmp(message["cmd"].as<char *>(), "debug") == 0){
    Serial.println("Received message debug mode ");
    mqttCmdDebug(message);
  }

  if(strcmp(message["cmd"].as<char *>(), "setProgram") == 0){
    Serial.println("Received Led program switch message");
    switchLedProgram(message);
  }
}

/*
 * Resets the WiFi config
 * mqtt Package:   
  {
    type: "cmd",
    cmd: "wifiReset"
  }
 */
void mqttCmdWiFiReset(){
  WiFiManager wifiManager;
  
  // delete config file
  if (SPIFFS.exists("/config.json")) {
    SPIFFS.remove("/config.json");
  }
  // Reset wifi
  wifiManager.resetSettings();
  // Start WiFi web page for new configuration
  ESP.restart();
}

/*
 * Debug messages over mqtt
 * mqtt Package:   
  {
    type: "cmd",
    cmd: "debug",
    value: "on"
  }
 */
void mqttCmdDebug(const JsonDocument& message){
  if(strcmp(message["value"], "on") == 0){
    Serial.println("Activate debug");
    mqttDebugActive = true;
  } else {
    Serial.println("Deactivate debug");
    mqttDebugActive = false; 
  }
}

/*
 * Debug messages over mqtt
 * mqtt Package:   
  {
    type: "cmd",
    cmd: "setProgram",
    value: "BALL/MANUAL"
  }
 */
void switchLedProgram(const JsonDocument& message){
  if(strcmp(message["value"], "BALL") == 0){
    Serial.println("Led program switch to BALL");
    activeProgram = BALL;
  } 
  if(strcmp(message["value"], "MANUAL") == 0){
    Serial.println("Led program switch to BALL");
    activeProgram = MANUAL;
    red = message["red"];
    green = message["green"];
    blue = message["blue"];
  } 
  if(strcmp(message["value"], "BLINK") == 0){
    Serial.println("Led program switch to BALL");
    activeProgram = BLINK;
  } 
}
