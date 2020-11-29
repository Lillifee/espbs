#include "WaveshareHelper.h"
#include "WebServerHelper.h"
#include "WifiHelper.h"
#include "rom/rtc.h"

RESET_REASON resetReason;

void deepSleep() {
  WiFiHelper.sleep();
  WaveshareHelper.sleep();
}

void setup() {
  Serial.begin(115200);

  WiFiHelper.setup();
  WaveshareHelper.setup();

  resetReason = rtc_get_reset_reason(0);

  // Enter configuration mode on reset
  if (resetReason == POWERON_RESET) {
    pinMode(LED_BUILTIN, OUTPUT);

    WiFiHelper.server();
    WaveshareHelper.server();

    WebServerHelper.onSleep(deepSleep);
    WebServerHelper.start("display");

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
