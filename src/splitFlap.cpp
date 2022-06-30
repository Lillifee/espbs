#include "SplitFlapHelper.h"
#include "WebServerHelper.h"
#include "WifiHelper.h"

void setup() {
  Serial.begin(115200);

  WiFiHelper.setup();
  WiFiHelper.server();

  SplitFlapHelper.setup();
  SplitFlapHelper.server();

  WebServerHelper.start("co2");
}

void loop() {
  SplitFlapHelper.loop();
}
