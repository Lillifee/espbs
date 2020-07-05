#ifndef WiFiHelper_h
#define WiFiHelper_h

#include <WiFi.h>
#include <WiFiClient.h>
#include <ESPmDNS.h>
#include <esp_wifi.h>
#include <Preferences.h>

#define DEFAULT_HOST "esp32"
#define DEFAULT_SSID "ESP32"
#define DEFAULT_PSK "password"

class WiFiHelperClass
{
private:
  unsigned long wifiTime;
  Preferences preferences;

public:
  String ssid;
  String password;
  String host;

  String mode;
  String ipv4;
  String dns;
  String subnet;
  String gateway;

  unsigned long getWifiTime();

  void setupAP();
  void setupMDNS();
  bool connectWifi();
  void disconnectWifi();

  void read();
  void write();
};

extern WiFiHelperClass WiFiHelper;

#endif