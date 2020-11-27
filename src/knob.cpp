#include "KnobHelper.h"
#include "WebServerHelper.h"
#include "WifiHelper.h"

unsigned long sleepTime;
esp_sleep_wakeup_cause_t wakeupReason;

void deepSleep() {
  WiFiHelper.sleep();
  KnobHelper.sleep();

  Serial.println("sleep");
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_13, 0);
  esp_deep_sleep_start();
}

void setup() {
  Serial.begin(115200);

  WiFiHelper.setup();
  KnobHelper.setup();

  wakeupReason = esp_sleep_get_wakeup_cause();

  if (wakeupReason == ESP_SLEEP_WAKEUP_UNDEFINED) {
    WiFiHelper.server();
    KnobHelper.server();

    WebServerHelper.onSleep(deepSleep);
    WebServerHelper.start();
  } else if (!WiFiHelper.connect()) {
    deepSleep();
  }

  sleepTime = millis() + 2000;
}

void loop() {
  KnobHelper.loop();
  delay(50);

  if (wakeupReason == ESP_SLEEP_WAKEUP_UNDEFINED) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);

    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
  } else {
    if (KnobHelper.value != KnobHelper.prevValue)
      sleepTime = millis() + 2000;

    if (millis() >= sleepTime)
      deepSleep();
  }
}
