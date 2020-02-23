// mqtt Server default values
char mqttServer[40] = "10.10.10.10";
char mqttPort[6] = "1883";
char mqttUser[40] = "guest";
char mqttPassword[40] = "guest";
char mqttNodeName[12] = "nodename";
char mqttRoomName[12] = "room";
char mqttDeviceType[12] = "devicetype";

//flag for saving data
bool shouldSaveConfig = false;

//callback notifying us of the need to save config
void saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}

void WiFIManagerInit(){
delay(200);
if (SPIFFS.exists("/config.json")) {
    //file exists, reading and loading
    Serial.println("reading config file");
    File configFile = SPIFFS.open("/config.json", "r");
    if (configFile) {
      Serial.println("opened config file");
      size_t size = configFile.size();
      // Allocate a buffer to store contents of the file.
      std::unique_ptr<char[]> buf(new char[size]);

      configFile.readBytes(buf.get(), size);
      DynamicJsonDocument json(1024);
      //JsonObject json = jsonBuffer.parseObject(buf.get());
      auto error = deserializeJson(json, buf.get());
      serializeJson(json, Serial);
      if (!error) {
        Serial.println("\nparsed json");

        strcpy(mqttServer, json["mqttServer"]);
        strcpy(mqttPort, json["mqttPort"]);
        strcpy(mqttUser, json["mqttUser"]);
        strcpy(mqttPassword, json["mqttPassword"]);
        strcpy(mqttNodeName, json["mqttNodeName"]);
        strcpy(mqttRoomName, json["mqttRoomName"]);
        strcpy(mqttDeviceType, json["mqttDeviceType"]);
      } else {
        Serial.println("failed to load json config");
      }
      configFile.close();
    }
  }

  // The extra parameters to be configured (can be either global or just in the setup)
  // After connecting, parameter.getValue() will get you the configured value
  // id/name placeholder/prompt default length
  WiFiManagerParameter customMqttServer("server", "mqtt server", mqttServer, 40);
  WiFiManagerParameter customMqttPort("port", "mqtt port", mqttPort, 6);
  WiFiManagerParameter customMqttUser("User", "mqtt user", mqttUser, 40);
  WiFiManagerParameter customMqttPassword("Password", "mqtt pasword", mqttPassword, 40);
  WiFiManagerParameter customMqttNodeName("NodeName", "node name", mqttNodeName, 12);  
  WiFiManagerParameter customMqttRoomName("RoomName", "room name", mqttRoomName, 12);  
  WiFiManagerParameter customMqttDeviceType("DeviceType", "device type", mqttDeviceType, 12);  
  
  WiFiManager wifiManager;      //Initialize the WiFi Manager

  //set config save notify callback
  wifiManager.setSaveConfigCallback(saveConfigCallback);
  
  //add all parameters here
  wifiManager.addParameter(&customMqttServer);
  wifiManager.addParameter(&customMqttPort);
  wifiManager.addParameter(&customMqttUser);
  wifiManager.addParameter(&customMqttPassword);
  wifiManager.addParameter(&customMqttNodeName);
  wifiManager.addParameter(&customMqttRoomName);
  wifiManager.addParameter(&customMqttDeviceType);

  //reset settings - for testing
  //wifiManager.resetSettings();

  wifiManager.autoConnect("AutoConnectAP");// use ESP Chip ID
  //if you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");

  //read updated parameters
  strcpy(mqttServer, customMqttServer.getValue());
  strcpy(mqttPort, customMqttPort.getValue());
  strcpy(mqttUser, customMqttUser.getValue());
  strcpy(mqttPassword, customMqttPassword.getValue());
  strcpy(mqttNodeName, customMqttNodeName.getValue());
  strcpy(mqttRoomName, customMqttRoomName.getValue());
  strcpy(mqttDeviceType, customMqttDeviceType.getValue());
  
  //save the custom parameters to FS
  if (shouldSaveConfig) {
    Serial.println("saving config");
    DynamicJsonDocument json(1024);
    //JsonObject json = jsonBuffer.createObject();
    json["mqttServer"] = mqttServer;
    json["mqttPort"] = mqttPort;
    json["mqttUser"] = mqttUser;
    json["mqttPassword"] = mqttPassword;
    json["mqttNodeName"] = mqttNodeName;
    json["mqttRoomName"] = mqttRoomName;
    json["mqttDeviceType"] = mqttDeviceType;

    File configFile = SPIFFS.open("/config.json", "w");
    if (!configFile) {
      Serial.println("failed to open config file for writing");
    }

    //json.printTo(Serial);
    serializeJson(json, Serial);
    //json.printTo(configFile);
    serializeJson(json, configFile);
    configFile.close();
    //end save
  }
/*
  Serial.println("***MQTT Parameter***");
  Serial.print("Server: ");
  Serial.println(mqttServer);
  Serial.print("Port: ");
  Serial.println(mqttPort);
  Serial.print("User: ");
  Serial.println(mqttUser);
  Serial.print("Password: ");
  Serial.println(mqttPassword);
*/
}
