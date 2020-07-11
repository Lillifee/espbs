#ifndef WiFiHelper_h
#define WiFiHelper_h

#include "AsyncJson.h"
#include "ESPmDNS.h"
#include "Preferences.h"
#include "WebServerHelper.h"
#include "WiFi.h"
#include "WiFiAP.h"
#include "WiFiClient.h"
#include "esp_wifi.h"

#define DEFAULT_HOST "esp32"
#define DEFAULT_SSID "ESP32"
#define DEFAULT_PSK "password"

class WiFiHelperClass {
 private:
  unsigned long wifiTime;
  Preferences preferences;

  String ssid;
  String password;
  String host;

  String mode;
  String ipv4;
  String dns;
  String subnet;
  String gateway;

  void read();
  void write();

  void setupAP();
  void setupMDNS();

 public:
  void server();
  bool connect(bool firstConnect = false);
  void sleep();
};

extern WiFiHelperClass WiFiHelper;

#endif