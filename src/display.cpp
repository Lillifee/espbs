#include "WaveshareHelper.h"
#include "WebServerHelper.h"
#include "WifiHelper.h"

esp_sleep_wakeup_cause_t wakeupReason;

void deepSleep() {
  WiFiHelper.sleep();
  WaveshareHelper.sleep();
}

void setup() {
  Serial.begin(115200);

  WiFiHelper.setup();
  WaveshareHelper.setup();

  wakeupReason = esp_sleep_get_wakeup_cause();

  // Enter configuration mode if the wakeup cause is undefined.
  if (wakeupReason == ESP_SLEEP_WAKEUP_UNDEFINED) {
    pinMode(LED_BUILTIN, OUTPUT);

    WiFiHelper.server();
    WaveshareHelper.server();

    WebServerHelper.onSleep(deepSleep);
    WebServerHelper.start();

    WaveshareHelper.update(false);
    return;
  }

  if (WiFiHelper.connect()) {
    WaveshareHelper.update(true);
  }

  deepSleep();
}

void loop() {
  // Loop is only executed in configuration mode.
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);

  digitalWrite(LED_BUILTIN, LOW);
  delay(500);
}
