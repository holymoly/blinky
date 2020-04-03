#ifdef ARDUINO_ARCH_ESP8266
  WiFiEventHandler wifiConnectHandler;
  WiFiEventHandler wifiDisconnectHandler;
#endif

Ticker wifiReconnectTimer;
Ticker mqttReconnectTimer;

void connectToWifi() {
  Serial.println("Connecting to Wi-Fi...");
  WiFIManagerInit();
}

#ifdef ARDUINO_ARCH_ESP32
  void onWifiConnect(WiFiEvent_t event, WiFiEventInfo_t info) {
#elif defined(ARDUINO_ARCH_ESP8266)
  void onWifiConnect(const WiFiEventStationModeGotIP& event) {
#endif
  Serial.println("Connected to Wi-Fi.");
  connectToMqtt();
}

#ifdef ARDUINO_ARCH_ESP32
  void onWifiDisconnect(WiFiEvent_t event, WiFiEventInfo_t info) {
#elif defined(ARDUINO_ARCH_ESP8266)
  void onWifiDisconnect(const WiFiEventStationModeDisconnected& event) {
#endif
  Serial.println("Disconnected from Wi-Fi.");
  mqttReconnectTimer.detach(); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
  wifiReconnectTimer.once(2, connectToWifi);
}

void connectToMqtt() {
  if(!mqttClient.connected()){
    Serial.println("Connecting to MQTT...");
    mqttClient.connect();
  }
}

void onMqttConnect(bool sessionPresent) {
  char topic[300]; // buffer for storing topics
  
  Serial.println("Connected to MQTT.");
  Serial.print("Session present: ");
  Serial.println(sessionPresent);

  // Listening on everything in topic light/
  if(snprintf (topic, 299, "%s/", mqttDeviceType) > 0){
    Serial.print("Subscribe to; ");
    Serial.println(topic);
    uint16_t packetIdSub = mqttClient.subscribe(topic, 2);
    Serial.print("Subscribing at QoS 2, packetId: ");
    Serial.println(packetIdSub);
  }

  // Listening on everything in topic light/space/
  if(snprintf (topic, 299, "%sspace/", topic) > 0){
    Serial.print("Subscribe to; ");
    Serial.println(topic);
    uint16_t packetIdSub = mqttClient.subscribe(topic, 2);
    Serial.print("Subscribing at QoS 2, packetId: ");
    Serial.println(packetIdSub);
  }

  // // Listening on everything in topic light/space/room/
  if(snprintf (topic, 299, "%s%s/", topic, mqttRoomName) > 0){
    Serial.print("Subscribe to; ");
    Serial.println(topic);
    uint16_t packetIdSub = mqttClient.subscribe(topic, 2);
    Serial.print("Subscribing at QoS 2, packetId: ");
    Serial.println(packetIdSub);
  }

  // // Listening on everything in topic light/space/room/nodename
  if(snprintf (topic, 299, "%s%s/", topic, mqttNodeName) > 0){
    Serial.print("Subscribe to; ");
    Serial.println(topic);
    uint16_t packetIdSub = mqttClient.subscribe(topic, 2);
    Serial.print("Subscribing at QoS 2, packetId: ");
    Serial.println(packetIdSub);

    // set last will testatment
    // this message will be send by the broker if the node has disconnected ungracefully
    mqttLwt(topic, 2);
  }
  
  StaticJsonDocument<50> doc;
  char jsonString[50];

  doc["state"] = "node connected";
  
  serializeJson(doc, jsonString);

  mqttClient.publish(mqttNodeName, 0, true, jsonString);
  Serial.println("Publishing at QoS 0");
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  Serial.println("Disconnected from MQTT.");

  if (WiFi.isConnected()) {
    mqttReconnectTimer.attach(2, connectToMqtt);
  }
}

void onMqttSubscribe(uint16_t packetId, uint8_t qos) {
  Serial.println("Subscribe acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
  Serial.print("  qos: ");
  Serial.println(qos);
}

void onMqttUnsubscribe(uint16_t packetId) {
  Serial.println("Unsubscribe acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}

void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
  char payloadFixedLength[len+1];
  payloadFixedLength[len] = '\0';
  strncpy(payloadFixedLength, payload, len);

  Serial.println("MQTT Message Received");
  Serial.println(payloadFixedLength);
  if(payloadFixedLength[0] == '{'){
    Serial.println("Payload could be json");

    // ***Parse json***
    DynamicJsonDocument doc(2048);
    // Deserialize the JSON document in the response
    DeserializationError err = deserializeJson(doc, payloadFixedLength);
    if (err) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(err.c_str());
    }

    serializeJson(doc, Serial);

    // check if type is cmd
    if(strcmp(doc["type"].as<char *>(), "cmd") == 0){
      Serial.println("Command received");
      
      parseMqttCommand(doc);
    }

    // check if type is req
    if(strcmp(doc["type"].as<char *>(), "req") == 0){
      Serial.println("Request received");
 
      parseMqttRequest(doc);
    }
  }
  
  Serial.println("Publish received.");
  Serial.print("  topic: ");
  Serial.println(topic);
  Serial.print("  qos: ");
  Serial.println(properties.qos);
  Serial.print("  dup: ");
  Serial.println(properties.dup);
  Serial.print("  retain: ");
  Serial.println(properties.retain);
  Serial.print("  len: ");
  Serial.println(len);
  Serial.print("  index: ");
  Serial.println(index);
  Serial.print("  total: ");
  Serial.println(total);
  Serial.print("  payload: ");
  Serial.println(payloadFixedLength);
}

void onMqttPublish(uint16_t packetId) {
  Serial.println("Publish acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}

void mqttInit(){
  uint16_t port;
  
  #ifdef ARDUINO_ARCH_ESP32
    WiFi.onEvent(onWifiConnect, SYSTEM_EVENT_STA_GOT_IP);
    WiFi.onEvent(onWifiDisconnect, SYSTEM_EVENT_STA_DISCONNECTED);
  #elif defined(ARDUINO_ARCH_ESP8266)
    wifiConnectHandler = WiFi.onStationModeGotIP(onWifiConnect);
    wifiDisconnectHandler = WiFi.onStationModeDisconnected(onWifiDisconnect);
  #endif
  //mqttClient.setClientId(clientID);
  mqttClient.setCredentials(mqttUser,mqttPassword);
  mqttClient.setCleanSession(true);
  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  mqttClient.onSubscribe(onMqttSubscribe);
  mqttClient.onUnsubscribe(onMqttUnsubscribe);
  mqttClient.onMessage(onMqttMessage);
  mqttClient.onPublish(onMqttPublish);
  sscanf (mqttPort,"%d",&port); // port must be an integer
  mqttClient.setServer(mqttServer, port);
}
