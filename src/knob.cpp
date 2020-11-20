#include "KnobHelper.h"
#include "WebServerHelper.h"
#include "WifiHelper.h"

unsigned long sleepTime;
RTC_DATA_ATTR int bootCount = 0;

void deepSleep() {
  WiFiHelper.sleep();
  KnobHelper.sleep();

  Serial.println("sleep");
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_13, 0);
  esp_deep_sleep_start();
}

void setup() {
  ++bootCount;
  setCpuFrequencyMhz(80);

  Serial.begin(115200);
  Serial.print("bootCount ");
  Serial.println(String(bootCount));

  WiFiHelper.setup();
  KnobHelper.setup();

  if (bootCount == 1) {
    pinMode(LED_BUILTIN, OUTPUT);

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

  if (bootCount != 1) {
    if (KnobHelper.value != KnobHelper.prevValue)
      sleepTime = millis() + 2000;

    if (millis() >= sleepTime)
      deepSleep();
  }
}
