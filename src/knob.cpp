#include "KnobHelper.h"
#include "WebServerHelper.h"
#include "WifiHelper.h"
#include "rom/rtc.h"

unsigned long sleepTime;
RESET_REASON resetReason;

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

  resetReason = rtc_get_reset_reason(0);

  if (resetReason == POWERON_RESET) {
    WiFiHelper.server();
    KnobHelper.server();

    WebServerHelper.onSleep(deepSleep);
    WebServerHelper.start("knob");
  } else if (!WiFiHelper.connect()) {
    deepSleep();
  }

  sleepTime = millis() + 2000;
}

void loop() {
  KnobHelper.loop();
  delay(50);

  if (resetReason == POWERON_RESET) {
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
