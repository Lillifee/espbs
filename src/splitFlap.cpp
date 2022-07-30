#include "SplitFlapHelper.h"
#include "WebServerHelper.h"
#include "WifiHelper.h"

void setup() {
  Serial.begin(115200);

  WiFiHelper.setup();
  WiFiHelper.server();

  SplitFlapHelper.setup();
  SplitFlapHelper.server();

  WebServerHelper.start("splitFlap");
}

void loop() {
  SplitFlapHelper.loop();
}
