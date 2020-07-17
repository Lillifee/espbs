#include "MpuHelper.h"
#include "WebServerHelper.h"
#include "WifiHelper.h"

#define LED_BUILTIN 5
unsigned long sleepTime;
RTC_DATA_ATTR int bootCount = 0;

// Upload the application
// pio run -t upload

// Upload the website
// pio run -t uploadfs

void deepSleep() {
  WiFiHelper.sleep();
  MpuHelper.sleep();

  esp_sleep_enable_ext0_wakeup(GPIO_NUM_33, 1);
  esp_deep_sleep_start();
}

void setup() {
  ++bootCount;
  setCpuFrequencyMhz(80);

  Serial.begin(115200);
  Serial.print("bootCount ");
  Serial.println(String(bootCount));

  WiFiHelper.setup();
  MpuHelper.setup();

  if (bootCount == 1) {
    pinMode(LED_BUILTIN, OUTPUT);

    WiFiHelper.server();
    MpuHelper.server();

    WebServerHelper.onSleep(deepSleep);
    WebServerHelper.start();
  } else if (!WiFiHelper.connect()) {
    deepSleep();
  }

  sleepTime = millis() + 1000;
}

void loop() {
  MpuHelper.loop();

  if (bootCount == 1) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);

    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
  } else {
    if (MpuHelper.prevSide != MpuHelper.side)
      sleepTime = millis() + 1000;

    if (millis() >= sleepTime)
      deepSleep();
  }
}
