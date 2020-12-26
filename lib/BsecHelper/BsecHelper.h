
#ifndef BsecHelper_h
#define BsecHelper_h

#include "Arduino.h"
#include "ArduinoJson.h"
#include "AsyncJson.h"
#include "HTTPClient.h"
#include "Preferences.h"
#include "WebServerHelper.h"
#include "WiFiUdp.h"
#include "WifiHelper.h"
#include "bsec.h"

#define STATE_SAVE_PERIOD UINT32_C(360 * 60 * 1000)  // 360 minutes - 4 times a day

class BsecHelperClass {
 private:
  WiFiUDP udp;
  Preferences preferences;

  int32_t requestInterval;
  unsigned long readTimer = 0;
  unsigned long requestTimer = 0;

  uint8_t bsecState[BSEC_MAX_STATE_BLOB_SIZE] = {0};
  uint16_t stateUpdateCounter = 0;

  String host;
  int32_t port;
  bool hasState;
  float tempOffset;

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
  Bsec iaqSensor;

  void setup(uint8_t i2cAddr);
  void server();
  void loop();
  void sleep();
};

extern BsecHelperClass BsecHelper;

#endif