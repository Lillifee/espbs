#include "MqttHelper.h"

const char* mqtt_server = "home.mybuddy.at";

MqttHelperClass::MqttHelperClass() : pubSubClient(wifiClient) {}

typedef struct pinTopic_ {
   int pin;
   char* topic;
} pinTopic;

pinTopic pins[] = {
   {25, "esp32/output1"},
   {26, "esp32/output2"},
   {27, "esp32/output3"},
   {14, "esp32/output4"}
};

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  for (unsigned int i = 0; i < sizeof(pins); i++)
  {
    if(String(topic) == pins[i].topic) {
      int value = messageTemp == "on" ? 1: 0;
      digitalWrite(pins[i].pin, value);
    }
  }  
}

void MqttHelperClass::setup() 
{
  for (unsigned int i = 0; i < sizeof(pins); i++)
  {
    pinMode(pins[i].pin, OUTPUT);
  }

  pubSubClient.setServer(mqtt_server,1883);
  pubSubClient.setCallback(callback);
}

bool MqttHelperClass::connect()
{
  if (pubSubClient.connected()) return true;

  // Attempt to connect
  Serial.print("Attempting MQTT connection...");
  if (pubSubClient.connect("ESP8266pubSubClient"))
  {
    Serial.println("connected");
    for (unsigned int i = 0; i < sizeof(pins); i++)
    {
      pubSubClient.subscribe(pins[i].topic);
    }
    return true;
  }

  Serial.print("failed, rc=");
  Serial.print(pubSubClient.state());
  return false;
}

void MqttHelperClass::publish(const char *topic, const char *payload) {
  pubSubClient.publish(topic, payload);
}


MqttHelperClass MqttHelper;