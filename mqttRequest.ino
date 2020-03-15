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

  const size_t capacity = JSON_ARRAY_SIZE(6) + 2*JSON_OBJECT_SIZE(2) + 3*JSON_OBJECT_SIZE(3) + 2*JSON_OBJECT_SIZE(6);
  DynamicJsonDocument doc(capacity);

  char jsonString[capacity];

  char topic[300];

  doc["type"] = "resp";
  doc["description"] = "You can use the message inside the wtf array for testing";
  doc["device"] = mqttNodeName;  
  doc["leds"] = NUMPIXELS; 
  doc["uid"] = message["uid"];

  JsonArray wtf = doc.createNestedArray("wtf");
  
  JsonObject wtf_0 = wtf.createNestedObject();
  wtf_0["type"] = "cmd";
  wtf_0["cmd"] = "setProgram";
  wtf_0["value"] = "BALL";
  
  JsonObject wtf_1 = wtf.createNestedObject();
  wtf_1["type"] = "cmd";
  wtf_1["cmd"] = "setProgram";
  wtf_1["value"] = "BLINK";
  
  JsonObject wtf_2 = wtf.createNestedObject();
  wtf_2["type"] = "cmd";
  wtf_2["cmd"] = "setProgram";
  wtf_2["value"] = "MANUAL";
  wtf_2["red"] = "125";
  wtf_2["green"] = "125";
  wtf_2["blue"] = "125";
  
  JsonObject wtf_3 = wtf.createNestedObject();
  wtf_3["type"] = "cmd";
  wtf_3["cmd"] = "setProgram";
  wtf_3["value"] = "RAINBOW";
  
  JsonObject wtf_4 = wtf.createNestedObject();
  wtf_4["type"] = "cmd";
  wtf_4["cmd"] = "wifiReset";
  
  JsonObject wtf_5 = wtf.createNestedObject();
  wtf_5["type"] = "cmd";
  wtf_5["cmd"] = "debug";

  serializeJson(doc, jsonString);

  //build topic
  //todo build topic string ones and not on every debug message
  if(snprintf (topic, 300, "%s/space/%s/%s/wtf/", mqttDeviceType, mqttRoomName, mqttNodeName) > 0){
    Serial.print("Send wtf message to: ");
    Serial.println(topic);
    mqttClient.publish(topic, 1, true, jsonString);
  }
}
