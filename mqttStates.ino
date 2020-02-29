void mqttDebug(char *message, int qos){
  if(mqttDebugActive){
    StaticJsonDocument<300> doc;
    char jsonString[300];
    char topic[300];
  
    doc["node"] = mqttNodeName;
    doc["type"] = "debug";
    doc["message"] = message;
  
    serializeJson(doc, jsonString);

    //build topic
    //todo build topic string ones and not on every debug message
    if(snprintf (topic, 299, "%s/space/%s/%s/debug/", mqttDeviceType, mqttRoomName, mqttNodeName) > 0){
      Serial.print("Send debug message to: ");
      Serial.println(topic);
      mqttClient.publish(topic, qos, true, jsonString);
    }
  }
}

// Setting Last Will Testament
void mqttLwt(char* topic, int qos){
  StaticJsonDocument<300> lastWill;
  char lastWillJsonString[300];

  lastWill["node"] = mqttNodeName;
  lastWill["type"] = "state";
  lastWill["state"] = "lwt"; // type last will
  lastWill["message"] = "Ungracefully dicsconeected";
  
  serializeJson(lastWill, lastWillJsonString);
  
  Serial.print("Set Last WIll Testament on topic: ");
  Serial.println(topic);
  mqttClient.setWill(topic, qos, true, lastWillJsonString);
}
