
// #include "WiFiHelper.h"
// #include "WebServerHelper.h"
// #include "UltrasonicHelper.h"
// #include "MqttHelper.h"

// #define sendDistInterval 10000;
// unsigned long sendDistTime = millis() + 1000;

// void setup()
// {
//   Serial.begin(115200);
//   delay(500);

//   WebServerHelper.start();
//   MqttHelper.setup();
//   UltrasonicHelper.setup();
// }

// void loop()
// {
//   if(!WiFiHelper.connectWifi()) {
//     Serial.printf("WiFi disconnected...");
//     return;
//   }

//   if(!MqttHelper.connect()) {
//     Serial.printf("MQTT disconnected...");
//     return;
//   }

//   if(millis() >= sendDistTime) {
//     float distance = UltrasonicHelper.readDistanceAverage();
//     const char * distanceString = UltrasonicHelper.convertToString(distance);
//     MqttHelper.publish("esp32/distance", distanceString);
//     sendDistTime = millis() + sendDistInterval;
//   }
// }
