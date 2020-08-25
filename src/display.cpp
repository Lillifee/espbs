#include "WaveshareHelper.h"
#include "WebServerHelper.h"
#include "WifiHelper.h"

RTC_DATA_ATTR int bootCount = 0;
long sleepDuration = 60;

// Upload the application
// pio run -t upload

// Upload the website
// pio run -t uploadfs
void deepSleep() {
  WiFiHelper.sleep();
  WaveshareHelper.sleep();

  esp_sleep_enable_timer_wakeup(1000 * 1000 * 1000 * 60 * sleepDuration);
  esp_deep_sleep_start();
}

void setup() {
  ++bootCount;
  setCpuFrequencyMhz(80);

  Serial.begin(115200);
  Serial.print("bootCount ");
  Serial.println(String(bootCount));

  WiFiHelper.setup();
  WaveshareHelper.setup();

  if (bootCount == 1) {
    pinMode(LED_BUILTIN, OUTPUT);

    WiFiHelper.server();
    WaveshareHelper.server();

    WebServerHelper.onSleep(deepSleep);
    WebServerHelper.start();

    WaveshareHelper.loop();
  } else if (!WiFiHelper.connect()) {
    deepSleep();
  }
}

void loop() {
  if (bootCount == 1) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);

    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
  } else {
    WaveshareHelper.loop();
    deepSleep();
  }
}
