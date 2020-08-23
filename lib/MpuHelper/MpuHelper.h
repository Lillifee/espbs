
#ifndef MpuHelper_h
#define MpuHelper_h

#include "ArduinoJson.h"
#include "AsyncJson.h"
#include "I2Cdev.h"
#include "MPU6050.h"
#include "WebServerHelper.h"
#include "WiFiUdp.h"
#include "Wire.h"

class MpuHelperClass {
 private:
  WiFiUDP udp;
  MPU6050 mpu6050;
  Preferences preferences;

  int16_t ax, ay, az;
  int16_t gx, gy, gz;

  String host;
  int32_t port;

  void read();
  void write();

  int calculateAxis(int16_t &value);
  void calculateSide();
  void logSettings();
  void readValues();
  void sendValues();

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