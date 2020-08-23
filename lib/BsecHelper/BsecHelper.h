
#ifndef BsecHelper_h
#define BsecHelper_h

#include "Arduino.h"
#include "ArduinoJson.h"
#include "AsyncJson.h"
#include "HTTPClient.h"
#include "WebServerHelper.h"
#include "WiFiUdp.h"
#include "bsec.h"

#define SEALEVELPRESSURE_HPA (1013.25)

class BsecHelperClass {
 private:
  WiFiUDP udp;
  Bsec iaqSensor;
  Preferences preferences;

  int32_t requestInterval;
  unsigned long readTimer = 0;
  unsigned long requestTimer = 0;

  String host;
  int32_t port;

  void read();
  void write();

  void checkSensor();
  void readValues();
  void sendValues();

  unsigned long requestDuration;
  unsigned long setupDuration;

 public:
  void setup();
  void server();
  void loop();
  void sleep();
};

extern BsecHelperClass BsecHelper;

#endif