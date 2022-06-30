#include "Arduino.h"
#include "BsecHelper.h"
#include "LedHelper.h"
#include "WebServerHelper.h"
#include "WifiHelper.h"

void setup() {
  Serial.begin(115200);

  WiFiHelper.setup();
  WiFiHelper.server();

  LedHelper.setup();
  LedHelper.server();

  BsecHelper.setup(BME680_I2C_ADDR_SECONDARY);
  BsecHelper.server();

  WebServerHelper.start("aqiLed");
}

void loop() {
  // Stop bsec readings during animation or test
  if (LedHelper.animation == 0 && LedHelper.test == 0) {
    BsecHelper.loop();
  }

  // Reconnect to Wifi on connection lost
  WiFiHelper.connect();

  LedHelper.value = BsecHelper.iaqSensor.iaq + 0.5;
  LedHelper.loop();
}
