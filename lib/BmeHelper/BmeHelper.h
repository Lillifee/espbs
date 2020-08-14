
#ifndef BmeHelper_h
#define BmeHelper_h

#include "Adafruit_BME680.h"
#include "Adafruit_Sensor.h"
#include "Arduino.h"
#include "ArduinoJson.h"
#include "AsyncJson.h"
#include "HTTPClient.h"
#include "SPI.h"
#include "WebServerHelper.h"
#include "WiFiUdp.h"

#define SEALEVELPRESSURE_HPA (1013.25)

class BmeHelperClass {
 private:
  WiFiUDP udp;
  Adafruit_BME680 bme;
  Preferences preferences;

  int32_t requestInterval;
  unsigned long readTimer = 0;
  unsigned long requestTimer = 0;

  String host;
  int32_t port;

  void read();
  void write();

  void readValues();
  void sendValues();

  unsigned long requestDuration;
  unsigned long setupDuration;

 public:
  float temp, humidity, pressure, gas, altitude;

  void setup();
  void server();
  void loop();
  void sleep();
};

extern BmeHelperClass BmeHelper;

#endif