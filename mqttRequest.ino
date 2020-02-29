/*
 * If mqtt packe was type request
 */
void parseMqttRequest(const JsonDocument& message){
  serializeJson(message, Serial);

  // check if wtf message was received.
  // Used to provide Information about this node.
  if(strcmp(message["req"].as<char *>(), "wtf") == 0){
    Serial.println("Received message wtf");
    mqttReqWtf(message);
  }
}

/*
 * Announcing functions of teh node
 * mqtt Package:   
  {
    type: "req",
    req: "wtf",
    uid: "01234"
  }
 */
void mqttReqWtf(const JsonDocument& message){

  // Response can be easily created with 
  // https://arduinojson.org/v6/assistant/
  // Just adjust  doc["device"] = mqttNodeName 
  //              doc["uid"]  = message["uid"];

  const size_t capacity = JSON_ARRAY_SIZE(4) + JSON_OBJECT_SIZE(2) + 2*JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(5) + JSON_OBJECT_SIZE(6);
  DynamicJsonDocument doc(capacity);
  char jsonString[capacity];

  char topic[300];

  doc["type"] = "resp";
  doc["description"] = "You can use the message inside the wtf array for testing";
  doc["device"] = mqttNodeName;  
  doc["uid"] = message["uid"];

  JsonArray wtf = doc.createNestedArray("wtf");

  
  JsonObject wtf_0 = wtf.createNestedObject();
  wtf_0["type"] = "cmd";
  wtf_0["cmd"] = "setProgram";
  wtf_0["value"] = "BALL";
  JsonObject wtf_01 = wtf.createNestedObject();
  wtf_01["type"] = "cmd";
  wtf_01["cmd"] = "setProgram";
  wtf_01["value"] = "LAUFLICHT";
  wtf_01["red"] = "125";
  wtf_01["green"] = "125";
  wtf_01["blue"] = "125";
  wtf_01["Speed"] = "50";

  JsonObject wtf_02 = wtf.createNestedObject();
  wtf_02["type"] = "cmd";
  wtf_02["cmd"] = "setProgram";
  wtf_02["value"] = "RAINBOW";
  wtf_02["Speed"] = "50";
  
  JsonObject wtf_03 = wtf.createNestedObject();
  wtf_02["type"] = "cmd";
  wtf_02["cmd"] = "setProgram";
  wtf_02["value"] = "CHASE";
  wtf_01["Speed"] = "50";

  JsonObject wtf_1 = wtf.createNestedObject();
  wtf_0["type"] = "cmd";
  wtf_0["cmd"] = "setProgram";
  wtf_0["value"] = "BLINK";

  JsonObject wtf_2 = wtf.createNestedObject();
  wtf_1["type"] = "cmd";
  wtf_1["cmd"] = "setProgram";
  wtf_1["value"] = "MANUAL";
  wtf_1["red"] = "125";
  wtf_1["green"] = "125";
  wtf_1["blue"] = "125";

  JsonObject wtf_3 = wtf.createNestedObject();
  wtf_2["type"] = "cmd";
  wtf_2["cmd"] = "wifiReset";

  JsonObject wtf_4 = wtf.createNestedObject();
  wtf_3["type"] = "cmd";
  wtf_3["cmd"] = "debug";
  wtf_3["value"] = "on";

  serializeJson(doc, jsonString);

  //build topic
  //todo build topic string ones and not on every debug message
  if(snprintf (topic, 299, "%s/space/%s/%s/wtf/", mqttDeviceType, mqttRoomName, mqttNodeName) > 0){
    Serial.print("Send wtf message to: ");
    Serial.println(topic);
    mqttClient.publish(topic, 1, true, jsonString);
  }
}
