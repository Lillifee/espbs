#include "Arduino.h"
#include "BsecHelper.h"
#include "Co2Helper.h"
#include "WebServerHelper.h"
#include "WifiHelper.h"

void setup() {
  Serial.begin(115200);

  WiFiHelper.setup();
  WiFiHelper.server();

  Co2Helper.setup();
  Co2Helper.server();

  BsecHelper.setup();
  BsecHelper.server();

  WebServerHelper.start("co2");
}

void loop() {
  Co2Helper.loop();
  BsecHelper.loop();
}
