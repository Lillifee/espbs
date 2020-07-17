
#ifndef MpuHelper_h
#define MpuHelper_h

#include "ArduinoJson.h"
#include "AsyncJson.h"
#include "HTTPClient.h"
#include "I2Cdev.h"
#include "MPU6050.h"
#include "WebServerHelper.h"
#include "Wire.h"

class MpuHelperClass {
 private:
  MPU6050 mpu6050;
  Preferences preferences;

  int16_t ax, ay, az;
  int16_t gx, gy, gz;

  char urls[6][256];

  void read();
  void write();

  int calculateAxis(int16_t &value);
  void calculateSide();
  void logSettings();
  void readValues();
  void sendHttpRequest();

  unsigned long requestDuration;
  unsigned long setupDuration;

  int duration;
  int threshold;

 public:
  int prevSide;
  int side;

  void setup();
  void server();
  void loop();
  void sleep();
};

extern MpuHelperClass MpuHelper;

#endif