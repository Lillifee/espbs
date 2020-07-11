
#ifndef MpuHelper_h
#define MpuHelper_h

#include "HTTPClient.h"
#include "I2Cdev.h"
#include "MPU6050.h"
#include "Wire.h"

class MpuHelperClass {
 private:
  MPU6050 mpu6050;

  int16_t ax, ay, az;
  int16_t gx, gy, gz;

  int calculateAxis(int16_t value);
  void calculateSide();
  void logSettings();
  void readValues();
  void sendHttpRequest();

 public:
  String prevSide;
  String side;

  void setup();
  void loop();
  void sleep();
};

extern MpuHelperClass MpuHelper;

#endif