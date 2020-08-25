#include "WaveshareHelper.h"

static const uint8_t EPD_BUSY = 4;   // to EPD BUSY
static const uint8_t EPD_CS = 5;     // to EPD CS
static const uint8_t EPD_RST = 16;   // to EPD RST
static const uint8_t EPD_DC = 17;    // to EPD DC
static const uint8_t EPD_SCK = 18;   // to EPD CLK
static const uint8_t EPD_MISO = 19;  // Master-In Slave-Out not used, as no data from display
static const uint8_t EPD_MOSI = 23;  // to EPD DIN

GxEPD2_BW<GxEPD2_750_T7, GxEPD2_750_T7::HEIGHT> display(GxEPD2_750_T7(EPD_CS, EPD_DC, EPD_RST, EPD_BUSY));
U8G2_FOR_ADAFRUIT_GFX u8g2Fonts;  // https://github.com/olikraus/u8g2/wiki/fntlistall

void WaveshareHelperClass::read() {
  preferences.begin("waveshare", true);
  host = preferences.getString("host");
  preferences.end();
}

void WaveshareHelperClass::write() {
  preferences.begin("waveshare", false);
  preferences.putString("host", host);
  preferences.end();
}

void WaveshareHelperClass::setup() {
  unsigned long setupStartTime = millis();
  Serial.println("Setup Waveshare helper");

  read();
  initDisplay();

  setupDuration = millis() - setupStartTime;
  Serial.print("Setup Waveshare helper took ");
  Serial.println(setupDuration);
}

void WaveshareHelperClass::loop() {
  if (host.length() == 0) return;

  u8g2Fonts.setBackgroundColor(GxEPD_BLACK);
  u8g2Fonts.setForegroundColor(GxEPD_WHITE);
  display.fillRect(0, 0, 320, 480, GxEPD_BLACK);

  drawStatus("CO2", host + "/jdev/sps/io/CO2Status", 40, 80);
  drawStatus("Qualität", host + "/jdev/sps/io/IAQStatus", 40, 200);
  drawStatus("Feuchtigkeit", host + "/jdev/sps/io/BME680%20Humidity", 40, 320);

  u8g2Fonts.setBackgroundColor(GxEPD_WHITE);
  u8g2Fonts.setForegroundColor(GxEPD_BLACK);

  u8g2Fonts.setFont(u8g2_font_helvB14_tf);
  drawString(360, 30, "Allgemein", LEFT);
  drawString(360, 270, "Heizung", LEFT);

  drawUsage(host + "/jdev/sps/io/AVerbrauch/all", 360, 70);
  drawUsage(host + "/jdev/sps/io/HVerbrauch/all", 360, 310);

  display.display(false);  // Full screen update mode
}

void WaveshareHelperClass::drawStatus(String title, String url, int offsetX, int offsetY) {
  // Send request
  http.useHTTP10(true);
  http.begin(url);
  http.GET();

  // Parse response
  DynamicJsonDocument doc(2048);
  deserializeJson(doc, http.getStream());

  // Read values
  String statusText = doc["LL"]["value"];

  u8g2Fonts.setFont(u8g2_font_helvB10_tf);
  drawString(offsetX, offsetY, title, LEFT);

  u8g2Fonts.setFont(u8g2_font_helvB24_tf);
  drawString(offsetX, offsetY + 40, statusText, LEFT);

  // Disconnect
  http.end();
}

void WaveshareHelperClass::drawUsage(String url, int offsetX, int offsetY) {
  // Send request
  http.useHTTP10(true);
  http.begin(url);
  http.GET();

  // Parse response
  DynamicJsonDocument doc(2048);
  deserializeJson(doc, http.getStream());

  // Read values
  float usageC = doc["LL"]["output1"]["value"].as<float>() * 1000;
  float usage0 = doc["LL"]["output3"]["value"].as<float>();
  float usage1 = doc["LL"]["output4"]["value"].as<float>();
  float usage2 = doc["LL"]["output5"]["value"].as<float>();

  // Disconnect
  http.end();

  u8g2Fonts.setFont(u8g2_font_helvR24_tf);
  drawString(290 + offsetX, 80 + offsetY, String(usageC, 0) + " W", LEFT);

  u8g2Fonts.setFont(u8g2_font_helvB08_tf);
  drawString(290 + offsetX, 45 + offsetY, "Aktuell", LEFT);

  drawString(45 + offsetX, 10 + offsetY, "Vorgestern", CENTER);
  drawString(135 + offsetX, 10 + offsetY, "Gestern", CENTER);
  drawString(225 + offsetX, 10 + offsetY, "Heute", CENTER);

  display.drawLine(10 + offsetX, 110 + offsetY, 260 + offsetX, 110 + offsetY, GxEPD_BLACK);

  u8g2Fonts.setFont(u8g2_font_helvR14_tf);
  drawString(45 + offsetX, 130 + offsetY, String(usage2), CENTER);
  drawString(135 + offsetX, 130 + offsetY, String(usage1), CENTER);
  drawString(225 + offsetX, 130 + offsetY, String(usage0), CENTER);

  float maxUsage = max(usage2, usage1);
  maxUsage = max(maxUsage, usage0) / 100;

  float pUsage0 = usage0 / maxUsage * 0.7;
  float pUsage1 = usage1 / maxUsage * 0.7;
  float pUsage2 = usage2 / maxUsage * 0.7;

  display.fillRect(35 + offsetX, 111 + offsetY - pUsage2, 20, pUsage2, GxEPD_BLACK);
  display.fillRect(125 + offsetX, 111 + offsetY - pUsage1, 20, pUsage1, GxEPD_BLACK);
  display.fillRect(215 + offsetX, 111 + offsetY - pUsage0, 20, pUsage0, GxEPD_BLACK);
}

void WaveshareHelperClass::initDisplay() {
  display.init(115200, true, 2);

  SPI.end();
  SPI.begin(EPD_SCK, EPD_MISO, EPD_MOSI, EPD_CS);
  u8g2Fonts.begin(display);       // connect u8g2 procedures to Adafruit GFX
  u8g2Fonts.setFontMode(1);       // use u8g2 transparent mode (this is default)
  u8g2Fonts.setFontDirection(0);  // left to right (this is default)
  u8g2Fonts.setForegroundColor(GxEPD_BLACK);
  u8g2Fonts.setBackgroundColor(GxEPD_WHITE);
  u8g2Fonts.setFont(u8g2_font_helvB10_tf);  // select u8g2 font from here: https://github.com/olikraus/u8g2/wiki/fntlistall
  display.fillScreen(GxEPD_WHITE);
  display.setFullWindow();
}

void WaveshareHelperClass::drawString(int x, int y, String text, alignment align) {
  int16_t x1, y1;
  uint16_t w, h;

  display.setTextWrap(false);
  display.getTextBounds(text, x, y, &x1, &y1, &w, &h);

  if (align == RIGHT) x = x - w;
  if (align == CENTER) x = x - w / 2;

  u8g2Fonts.setCursor(x, y + h);
  u8g2Fonts.print(text);
}

void WaveshareHelperClass::sleep() {
  display.powerOff();

  // Write low to reset
  digitalWrite(SDA, LOW);
  digitalWrite(SCL, LOW);
}

void WaveshareHelperClass::server() {
  Serial.println("Setup Waveshare server");

  WebServerHelper.server.on("/api/waveshare", HTTP_GET, [this](AsyncWebServerRequest *request) {
    int args = request->args();

    if (args > 0) {
      request->send(200, "text/plain", "message received");
      Serial.println("Update Waveshare settings");

      if (request->hasArg("host")) host = request->arg("host");

      write();
      ESP.restart();

    } else {
      AsyncJsonResponse *response = new AsyncJsonResponse();
      response->addHeader("Server", "ESP Async Web Server");
      JsonVariant &root = response->getRoot();

      root["setupDuration"] = setupDuration;
      root["host"] = host;

      response->setLength();
      request->send(response);
    }
  });
}

WaveshareHelperClass WaveshareHelper;
