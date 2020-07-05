#ifndef MqttHelper_h
#define MqttHelper_h

#include <WiFiClient.h>
#include <PubSubClient.h>

class MqttHelperClass
{
private:
    WiFiClient wifiClient;
    PubSubClient pubSubClient;

public:
    MqttHelperClass();
    bool connect();
    void setup();
    void publish(const char *topic, const char *payload);
};

extern MqttHelperClass MqttHelper;

#endif