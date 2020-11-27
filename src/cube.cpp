#include "MpuHelper.h"
#include "WebServerHelper.h"
#include "WifiHelper.h"

#define LED_BUILTIN 5
unsigned long sleepTime;
esp_sleep_wakeup_cause_t wakeupReason;

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

  wakeupReason = esp_sleep_get_wakeup_cause();

  if (wakeupReason == ESP_SLEEP_WAKEUP_UNDEFINED) {
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

  if (wakeupReason == ESP_SLEEP_WAKEUP_UNDEFINED) {
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
