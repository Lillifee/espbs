#include "WebServerHelper.h"
#include "MpuHelper.h"
#include "HTTPClient.h"

const String urlPart1 = "http://webserver/dev/sps/io/ESP1-";
const String urlPart2 = "/pulse";

#define LED_BUILTIN 5

unsigned long sleepTime;

RTC_DATA_ATTR int bootCount = 0;

// Upload the application
// pio run -t upload

// Upload the website
// pio run -t uploadfs

// TODO replace with udp
void sendHttpRequest()
{
  if (MpuHelper.prevSide == MpuHelper.side)
    return;

  // This will send the request to the server
  HTTPClient http;
  String url = urlPart1 + MpuHelper.side + urlPart2;

  Serial.println(url);

  http.begin(url); 
  http.GET();      
  http.end();
}

void deepSleep()
{
  WiFiHelper.disconnectWifi();
  MpuHelper.sleep();

  esp_sleep_enable_ext0_wakeup(GPIO_NUM_33, 1);
  esp_deep_sleep_start();
}

void checkDeepSleep()
{
  if (MpuHelper.prevSide != MpuHelper.side)
    sleepTime = millis() + 1000;

  if (millis() >= sleepTime)
    deepSleep();
}

void setup()
{
  ++bootCount;
  setCpuFrequencyMhz(80);

  Serial.begin(115200);
  Serial.println("bootCount " + String(bootCount));

  MpuHelper.initialize();

  if (bootCount == 1)
  {
    pinMode(LED_BUILTIN, OUTPUT);
    WebServerHelper.start();
  }
  else if (!WiFiHelper.connectWifi())
  {
    deepSleep();
  }

  sleepTime = millis() + 1000;
}

void loop()
{
  if (WebServerHelper.isRunning())
  {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
    
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
  }
  else
  {
    MpuHelper.readValues();
    sendHttpRequest();
    checkDeepSleep();
  }
}
