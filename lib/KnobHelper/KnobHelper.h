
#ifndef KnobHelper_h
#define KnobHelper_h

#include "AiEsp32RotaryEncoder.h"
#include "ArduinoJson.h"
#include "AsyncJson.h"
#include "I2Cdev.h"
#include "MPU6050.h"
#include "Preferences.h"
#include "WebServerHelper.h"
#include "WiFiHelper.h"
#include "WiFiUdp.h"
#include "Wire.h"

/*
connecting Rotary encoder
CLK (A pin) - to any microcontroler intput pin with interrupt -> in this example pin 32
DT (B pin) - to any microcontroler intput pin with interrupt -> in this example pin 21
SW (button pin) - to any microcontroler intput pin -> in this example pin 25
VCC - to microcontroler VCC (then set ROTARY_ENCODER_VCC_PIN -1) or in this example pin 25
GND - to microcontroler GND
*/
#define ROTARY_ENCODER_A_PIN 15
#define ROTARY_ENCODER_B_PIN 4
#define ROTARY_ENCODER_BUTTON_PIN 13
#define ROTARY_ENCODER_VCC_PIN -1 /*put -1 of Rotary encoder Vcc is connected directly to 3,3V; else you can use declared output pin for powering rotary encoder */
#define ROTARY_ENCODER_TEST_LIMITS = 2;

class KnobHelperClass {
 private:
  WiFiUDP udp;
  Preferences preferences;

  String host;
  int32_t port;

  void read();
  void write();

  void readValues();
  void sendValues();

  unsigned long requestDuration;
  unsigned long setupDuration;

  int threshold;
  int testlimits = 2;

 public:
  int16_t value;
  int16_t prevValue;
  bool pressed;

  void setup();
  void server();
  void loop();
  void sleep();
};

extern KnobHelperClass KnobHelper;

#endif