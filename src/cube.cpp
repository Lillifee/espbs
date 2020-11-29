#include "MpuHelper.h"
#include "WebServerHelper.h"
#include "WifiHelper.h"
#include "rom/rtc.h"

#define LED_BUILTIN 5
unsigned long sleepTime;
RESET_REASON resetReason;

void deepSleep() {
  WiFiHelper.sleep();
  MpuHelper.sleep();

  esp_sleep_enable_ext0_wakeup(GPIO_NUM_33, 1);
  esp_deep_sleep_start();
}

void setup() {
  Serial.begin(115200);

  WiFiHelper.setup();
  MpuHelper.setup();

  resetReason = rtc_get_reset_reason(0);

  if (resetReason == POWERON_RESET) {
    pinMode(LED_BUILTIN, OUTPUT);

    WiFiHelper.server();
    MpuHelper.server();

    WebServerHelper.onSleep(deepSleep);
    WebServerHelper.start("cube");
  } else if (!WiFiHelper.connect()) {
    deepSleep();
  }

  sleepTime = millis() + 1000;
}

void loop() {
  MpuHelper.loop();

  if (resetReason == POWERON_RESET) {
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
