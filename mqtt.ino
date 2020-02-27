WiFiEventHandler wifiConnectHandler;
WiFiEventHandler wifiDisconnectHandler;

Ticker wifiReconnectTimer;

void connectToWifi() {
  Serial.println("Connecting to Wi-Fi...");
  WiFIManagerInit();
}

void onWifiConnect(const WiFiEventStationModeGotIP& event) {
  Serial.println("Connected to Wi-Fi.");
  connectToMqtt();
}

void onWifiDisconnect(const WiFiEventStationModeDisconnected& event) {
  Serial.println("Disconnected from Wi-Fi.");
  mqttReconnectTimer.detach(); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
  wifiReconnectTimer.once(2, connectToWifi);
}

void connectToMqtt() {
  Serial.println("Connecting to MQTT...");
  mqttClient.connect();
}

void onMqttConnect(bool sessionPresent) {
  char topic[300]; // buffer for storing topics
  //uint32_t chipid=ESP.getChipId();
  //char nodeTopic[12];
  // publishing on topic nodes/ESP.getChipId()
  //snprintf(nodeTopic,13,"nodes/%06X",chipid);
  
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
  if(snprintf (topic, 299, "%s%s", topic, mqttRoomName) > 0){
    Serial.print("Subscribe to; ");
    Serial.println(topic);
    uint16_t packetIdSub = mqttClient.subscribe(topic, 2);
    Serial.print("Subscribing at QoS 2, packetId: ");
    Serial.println(packetIdSub);
  }

  // // Listening on everything in topic light/space/room/nodename
  if(snprintf (topic, 299, "%s%s", topic, mqttNodeName) > 0){
    Serial.print("Subscribe to; ");
    Serial.println(topic);
    uint16_t packetIdSub = mqttClient.subscribe(topic, 2);
    Serial.print("Subscribing at QoS 2, packetId: ");
    Serial.println(packetIdSub);
  }

  mqttClient.publish(mqttNodeName, 0, true, "node connected");
  Serial.println("Publishing at QoS 0");
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  Serial.println("Disconnected from MQTT.");

  if (WiFi.isConnected()) {
    mqttReconnectTimer.once(2, connectToMqtt);
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
    if(strcmp(doc["type"].as<char *>(), "cmd") == 0){
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
  
  uint32_t chipid=ESP.getChipId();
  char clientID[6];
  // publishing on topic nodes/ESP.getChipId()
  snprintf(clientID,7,"%06X",chipid);
  
  wifiConnectHandler = WiFi.onStationModeGotIP(onWifiConnect);
  wifiDisconnectHandler = WiFi.onStationModeDisconnected(onWifiDisconnect);
  mqttClient.setClientId(clientID);
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
