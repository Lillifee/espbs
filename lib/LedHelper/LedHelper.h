
#ifndef LedHelper_h
#define LedHelper_h

#include "Arduino.h"
#include "ArduinoJson.h"
#include "AsyncJson.h"
#include "FastLED.h"
#include "Preferences.h"
#include "WebServerHelper.h"

#define DATA_PIN 13
#define NUM_LEDS 24
#define LED_TYPE WS2812
#define COLOR_ORDER GRB

class LedHelperClass {
 private:
  Preferences preferences;
  CRGB leds[NUM_LEDS];

  int mode;
  int color;
  int brightness;
  int initHue;
  int deltaHue;
  int endHue;
  int displayValue;

  void read();
  void write();

  int valueToNumLed();
  int valueToHue();

  bool animate();
  void fillColorsFromPalette(uint8_t colorIndex);

  CRGBPalette16 currentPalette;
  TBlendType currentBlending;

 public:
  int value;
  int minValue;
  int maxValue;
  int test;
  int animation;

  void setup();
  void server();
  void loop();
  void sleep();
};

extern LedHelperClass LedHelper;

#endif