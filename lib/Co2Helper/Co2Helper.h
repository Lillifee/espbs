
#ifndef Co2Helper_h
#define Co2Helper_h

#include "Arduino.h"
#include "ArduinoJson.h"
#include "AsyncJson.h"
#include "HTTPClient.h"
#include "MHZ19.h"
#include "Preferences.h"
#include "WebServerHelper.h"
#include "WiFiHelper.h"
#include "WiFiUdp.h"

class Co2HelperClass {
 private:
  MHZ19 mhz19;
  WiFiUDP udp;
  Preferences preferences;

  int32_t requestInterval;
  unsigned long readTimer = 0;
  unsigned long requestTimer = 0;

  String host;
  int32_t port;
  boolean autoCalibration;

  void read();
  void write();

  void readValues();
  void sendValues();

  unsigned long requestDuration;
  unsigned long setupDuration;

 public:
  Co2HelperClass();
  HardwareSerial hwSerial;

  float temp;
  int co2;

  void setup();
  void server();
  void loop();
  void sleep();
};

extern Co2HelperClass Co2Helper;

#endif