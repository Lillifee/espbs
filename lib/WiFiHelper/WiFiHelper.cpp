#include "WiFiHelper.h"

void WiFiHelperClass::read() {
  preferences.begin("esp", false); // create folder

  ssid = preferences.getString("ssid", DEFAULT_SSID);
  password = preferences.getString("password", DEFAULT_PSK);
  host = preferences.getString("host", DEFAULT_HOST);

  mode = preferences.getString("mode", "dhcp");
  ipv4 = preferences.getString("ipv4");
  dns = preferences.getString("dns");
  subnet = preferences.getString("subnet");
  gateway = preferences.getString("gateway");

  preferences.end();
}

void WiFiHelperClass::write()
{
  preferences.begin("esp", false); // create folder

  preferences.putString("ssid", ssid);
  preferences.putString("password", password);
  preferences.putString("host", host);

  preferences.putString("mode", mode);
  preferences.putString("ipv4", ipv4);
  preferences.putString("dns", dns);
  preferences.putString("subnet", subnet);
  preferences.putString("gateway", gateway);

  preferences.end();
}

bool WiFiHelperClass::connectWifi()
{
  read();

  if (WiFi.status() == WL_CONNECTED)
    return true;
    
  unsigned long wifiStartTime = millis();

  if (mode == "static")
  {
    IPAddress ipv4IP;
    IPAddress dnsIP;
    IPAddress subnetIP;
    IPAddress gatewayIP;

    ipv4IP.fromString(ipv4);
    dnsIP.fromString(dns);
    subnetIP.fromString(subnet);
    gatewayIP.fromString(gateway);

    Serial.println("WIFI use static IP: " + ipv4);

    WiFi.config(ipv4IP, gatewayIP, subnetIP, dnsIP);
  }

  Serial.print("WIFI connect to " + ssid);

  // WiFi.mode(WIFI_STA);
  // WiFi.persistent(true);
  // WiFi.setAutoConnect(true);
  // WiFi.setAutoReconnect(true);
  WiFi.begin(ssid.c_str(), password.c_str());

  int retry = 0;
  while (WiFi.status() != WL_CONNECTED && retry < 50)
  {
    Serial.print(".");
    delay(10);
    retry++;
  }

  bool connected = WiFi.status() == WL_CONNECTED;
  wifiTime = millis() - wifiStartTime;
  Serial.println("WIFI connection took " + String(wifiTime));

  return connected;
}

void WiFiHelperClass::setupAP()
{
  // configure WiFi AP
  // TODO only if necessary
  WiFi.disconnect();
  WiFi.softAP("espbs");
  WiFi.persistent(true);

  IPAddress myIP = WiFi.softAPIP();
  Serial.println("AP IP address: " + myIP);
  Serial.println("password: " + String(DEFAULT_PSK));
}

void WiFiHelperClass::setupMDNS()
{
  /*use mdns for host name resolution*/
  if (!MDNS.begin(host.c_str()))
  {
    Serial.println("Error setting up MDNS responder!");
  }
  Serial.println("mDNS responder started connect to http://" + host + ".local");
}

void WiFiHelperClass::disconnectWifi()
{
  // TODO check if necessary
  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);

  btStop();
  esp_wifi_stop();
}

unsigned long WiFiHelperClass::getWifiTime() {
  return wifiTime;
}

WiFiHelperClass WiFiHelper;