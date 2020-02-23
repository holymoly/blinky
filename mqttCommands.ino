/*
 * If mqtt packe was type command
 */
void parseMqttCommand(const JsonDocument& message){

  serializeJson(message, Serial);

  if(strcmp(message["cmd"].as<char *>(), "wifiReset") == 0){
    mqttCmdWiFiReset();
  }

  if(strcmp(message["cmd"].as<char *>(), "debug") == 0){
    Serial.println("Received debug mode message");
    mqttCmdDebug(message);
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
