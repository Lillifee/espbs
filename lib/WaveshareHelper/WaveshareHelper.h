
#ifndef WaveshareHelper_h
#define WaveshareHelper_h
#define ENABLE_GxEPD2_display 1

#include "ArduinoJson.h"
#include "AsyncJson.h"
#include "GxEPD2_BW.h"
// #include "I2Cdev.h"
#include "HTTPClient.h"
#include "SPI.h"
#include "U8g2_for_Adafruit_GFX.h"
#include "WebServerHelper.h"
#include "WiFi.h"
#include "Wire.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 480
#define uS_TO_S_FACTOR 1000000

enum alignment { LEFT,
                 RIGHT,
                 CENTER };

class WaveshareHelperClass {
 private:
  Preferences preferences;
  HTTPClient http;
  String host;
  String user;
  String password;
  int32_t updateInterval;

  void read();
  void write();

  void readValues();
  void sendValues();

  unsigned long setupDuration;

  void initDisplay();

  bool getAndParse(JsonDocument &doc, String urlPart);
  void drawStatus(String title, String urlPart, int offsetX, int offsetY);
  void drawUsage(String title, String urlPart, int offsetX, int offsetY);
  void drawString(int x, int y, String text, alignment align);

 public:
  void setup();
  void server();
  void update(bool disconnect);
  void sleep();
};

extern WaveshareHelperClass WaveshareHelper;

#endif