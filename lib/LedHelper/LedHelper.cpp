#include "LedHelper.h"

void LedHelperClass::read() {
  preferences.begin("Led", true);
  mode = preferences.getInt("mode", 0);
  brightness = preferences.getInt("brightness", 100);
  color = preferences.getInt("color", 0x808000);
  minValue = preferences.getInt("minValue", 25);
  maxValue = preferences.getInt("maxValue", 350);
  initHue = preferences.getInt("initHue", 96);
  endHue = preferences.getInt("endHue", 0);
  deltaHue = preferences.getInt("deltaHue", -4);
  preferences.end();
}

void LedHelperClass::write() {
  preferences.begin("Led", false);
  preferences.putInt("mode", mode);
  preferences.putInt("brightness", brightness);
  preferences.putInt("color", color);
  preferences.putInt("minValue", minValue);
  preferences.putInt("maxValue", maxValue);
  preferences.putInt("initHue", initHue);
  preferences.putInt("deltaHue", deltaHue);
  preferences.end();
}

void LedHelperClass::setup() {
  read();
  value = 0;
  displayValue = 0;
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(brightness);
}

int LedHelperClass::valueToNumLed() {
  int numLedsToLight = map(displayValue, minValue, maxValue, 0, NUM_LEDS);
  numLedsToLight = max(0, min(numLedsToLight, NUM_LEDS));
  return numLedsToLight;
}

int LedHelperClass::valueToHue() {
  int hue = map(displayValue, minValue, maxValue, initHue, endHue);

  return hue;
}

void LedHelperClass::fillColorsFromPalette(uint8_t colorIndex) {
  uint8_t brightness = 255;

  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = ColorFromPalette(currentPalette, colorIndex, brightness, currentBlending);
    // TODO adjustable
    colorIndex += 3;
  }
}

bool LedHelperClass::animate() {
  switch (animation) {
    case 1:
      currentPalette = RainbowColors_p;
      currentBlending = LINEARBLEND;
      break;
    case 2:
      currentPalette = RainbowStripeColors_p;
      currentBlending = NOBLEND;
      break;
    case 3:
      currentPalette = CloudColors_p;
      currentBlending = LINEARBLEND;
      break;
    case 4:
      currentPalette = PartyColors_p;
      currentBlending = LINEARBLEND;
      break;
    case 5:
      currentPalette = OceanColors_p;
      currentBlending = LINEARBLEND;
      break;
    case 6:
      currentPalette = HeatColors_p;
      currentBlending = LINEARBLEND;
      break;
    case 7:
      currentPalette = ForestColors_p;
      currentBlending = LINEARBLEND;
      break;
    case 8:
      currentPalette = LavaColors_p;
      currentBlending = LINEARBLEND;
      break;

    default:
      return false;
  }

  static uint8_t startIndex = 0;
  // TODO adjustable
  startIndex = startIndex + 1; /* motion speed */

  fillColorsFromPalette(startIndex);

  FastLED.show();
  FastLED.delay(1000 / 100);

  return true;
}

void LedHelperClass::loop() {
  value = max(0, min(maxValue, value));

  if (test > 0) {
    if (test == 1) {
      value = maxValue;
      test++;
    } else {
      value = 0;
      test = 0;
    }
  }

  if (animate()) return;

  int startValue = displayValue;
  int range = abs(displayValue - value);
  for (int i = 0; i < range; i++) {
    fract8 f = (256 * i) / range;
    fract8 easeOutVal = ease8InOutQuad(f);
    displayValue = lerp16by8(startValue, value, easeOutVal);

    FastLED.clear();
    switch (mode) {
      case 0: {
        fill_rainbow(leds, valueToNumLed(), initHue, deltaHue);
        break;
      }
      case 1: {
        fill_solid(leds, valueToNumLed(), color);
        break;
      }
      case 2: {
        fill_solid(leds, NUM_LEDS, displayValue < minValue ? CHSV(0, 0, 0) : CHSV(valueToHue(), 255, 255));
        break;
      }
      default: {
        break;
      }
    }

    if (brightness == -1) {
      FastLED.setBrightness(map(displayValue, minValue, maxValue, 0, 255));
    } else {
      FastLED.setBrightness(brightness);
    }

    FastLED.show();
    FastLED.delay(5);
  }

  displayValue = value;
}

void LedHelperClass::sleep() {
}

void LedHelperClass::server() {
  Serial.println("Setup Led server");

  WebServerHelper.server.on("/api/led", HTTP_GET, [this](AsyncWebServerRequest *request) {
    int args = request->args();

    if (args > 0) {
      request->send(200, "text/plain", "message received");
      Serial.println("Update Led settings");

      if (request->hasArg("value")) value = request->arg("value").toInt();
      if (request->hasArg("brightness")) brightness = request->arg("brightness").toInt();
      if (request->hasArg("mode")) mode = request->arg("mode").toInt();
      if (request->hasArg("color")) color = request->arg("color").toInt();
      if (request->hasArg("min")) minValue = request->arg("min").toInt();
      if (request->hasArg("max")) maxValue = request->arg("max").toInt();
      if (request->hasArg("initHue")) initHue = request->arg("initHue").toInt();
      if (request->hasArg("deltaHue")) deltaHue = request->arg("deltaHue").toInt();
      if (request->hasArg("endHue")) endHue = request->arg("endHue").toInt();
      if (request->hasArg("animation")) animation = request->arg("animation").toInt();

      test = 1;
      write();

    } else {
      AsyncJsonResponse *response = new AsyncJsonResponse();
      response->addHeader("Server", "ESP Async Web Server");
      JsonVariant &root = response->getRoot();

      root["value"] = value;
      root["brightness"] = brightness;
      root["mode"] = mode;
      root["color"] = color;
      root["min"] = minValue;
      root["max"] = maxValue;
      root["initHue"] = initHue;
      root["deltaHue"] = deltaHue;
      root["endHue"] = endHue;
      root["animation"] = animation;

      response->setLength();
      request->send(response);
    }
  });
}

LedHelperClass LedHelper;
