#include "WaveshareHelper.h"
#include "WebServerHelper.h"
#include "WifiHelper.h"

RTC_DATA_ATTR int bootCount = 0;

void deepSleep() {
  WiFiHelper.sleep();
  WaveshareHelper.sleep();
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

    WaveshareHelper.update(false);
  } else {
    // Update if connected
    if (!WiFiHelper.connect()) {
      WaveshareHelper.update(true);
    }

    deepSleep();
  }
}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);

  digitalWrite(LED_BUILTIN, LOW);
  delay(500);
}
