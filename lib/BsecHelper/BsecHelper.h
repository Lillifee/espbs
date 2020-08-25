
#ifndef BsecHelper_h
#define BsecHelper_h

#include "Arduino.h"
#include "ArduinoJson.h"
#include "AsyncJson.h"
#include "HTTPClient.h"
#include "WebServerHelper.h"
#include "WiFiUdp.h"
#include "bsec.h"

#define STATE_SAVE_PERIOD UINT32_C(360 * 60 * 1000)  // 360 minutes - 4 times a day

class BsecHelperClass {
 private:
  WiFiUDP udp;
  Bsec iaqSensor;
  Preferences preferences;

  int32_t requestInterval;
  unsigned long readTimer = 0;
  unsigned long requestTimer = 0;

  uint8_t bsecState[BSEC_MAX_STATE_BLOB_SIZE] = {0};
  uint16_t stateUpdateCounter = 0;

  String host;
  int32_t port;
  bool hasState;

  void read();
  void write();

  void checkSensor();
  void readValues();
  void sendValues();

  void updateState();
  void writeState(bool write);

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