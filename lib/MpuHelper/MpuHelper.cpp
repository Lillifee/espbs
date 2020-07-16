#include "MpuHelper.h"

void MpuHelperClass::read() {
  preferences.begin("mpu", true);
  preferences.getBytes("sideUrl", urls, sizeof(urls));
  duration = preferences.getInt("duration", 1);
  threshold = preferences.getInt("threshold", 20);
  preferences.end();
}

void MpuHelperClass::write() {
  preferences.begin("mpu", false);
  preferences.putBytes("sideUrl", urls, sizeof(urls));
  preferences.putInt("duration", duration);
  preferences.putInt("threshold", threshold);
  preferences.end();
}

void MpuHelperClass::setup() {
  unsigned long setupStartTime = millis();
  Serial.println("Setup MPU helper");

  read();

  Wire.begin(SDA, SCL);

  // Reset the MPU (to try different settings)
  // mpu6050.reset();
  // delay(100);

  mpu6050.initialize();

  Serial.println("Testing device connections...");
  if (!mpu6050.testConnection()) {
    Serial.println("MPU6050 connection failed");
    return;
  }

  Serial.println("MPU6050 connection successful");

  // make sure accel is running
  mpu6050.setSleepEnabled(false);
  mpu6050.setWakeCycleEnabled(false);

  // enable accel
  mpu6050.setStandbyXAccelEnabled(false);
  mpu6050.setStandbyYAccelEnabled(false);
  mpu6050.setStandbyZAccelEnabled(false);

  // standby gyro
  mpu6050.setStandbyXGyroEnabled(true);
  mpu6050.setStandbyYGyroEnabled(true);
  mpu6050.setStandbyZGyroEnabled(true);

  // disable temperature sensor
  mpu6050.setTempSensorEnabled(false);

  // set accel HPF to reset settings
  mpu6050.setDHPFMode(0);

  // set accel LPF setting to 256hz bandwidth
  mpu6050.setDLPFMode(0);

  // enable motion interrupt
  mpu6050.setIntMotionEnabled(true);
  mpu6050.setMotionDetectionDuration(duration);    // 1
  mpu6050.setMotionDetectionThreshold(threshold);  // 20

  // logSettings();

  setupTime = millis() - setupStartTime;
  Serial.print("Setup MPU helper took ");
  Serial.println(setupTime);
}

void MpuHelperClass::loop() {
  readValues();
  sendHttpRequest();
}

void MpuHelperClass::sleep() {
  if (!mpu6050.testConnection()) {
    return;
  }

  // set accel HPF to hold
  mpu6050.setDHPFMode(7);

  // set the frequency to wakeup
  mpu6050.setWakeFrequency(0);

  mpu6050.setStandbyXGyroEnabled(true);  // should be already set
  mpu6050.setStandbyYGyroEnabled(true);  // should be already set
  mpu6050.setStandbyZGyroEnabled(true);  // should be already set

  mpu6050.setTempSensorEnabled(false);
  mpu6050.setWakeCycleEnabled(true);

  // Write low to MPU pins to fallback to 6.5uA
  digitalWrite(SDA, LOW);
  digitalWrite(SCL, LOW);
}

void MpuHelperClass::readValues() {
  mpu6050.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  Serial.print(ax);
  Serial.print("\t");
  Serial.print(ay);
  Serial.print("\t");
  Serial.println(az);

  calculateSide();
  sendHttpRequest();
}

int MpuHelperClass::calculateAxis(int16_t &value) {
  return value > 8000 ? 1 : value < -8000 ? -1 : 0;
}

void MpuHelperClass::calculateSide() {
  int sideX = calculateAxis(ax);
  int sideY = calculateAxis(ay);
  int sideZ = calculateAxis(az);

  prevSide = side;

  if (sideX == 0 && sideY == 0 && sideZ == 1) {
    side = 0;
  } else if (sideX == 0 && sideY == 0 && sideZ == -1) {
    side = 1;
  } else if (sideX == 0 && sideY == -1 && sideZ == 0) {
    side = 2;
  } else if (sideX == 0 && sideY == 1 && sideZ == 0) {
    side = 3;
  } else if (sideX == 1 && sideY == 0 && sideZ == 0) {
    side = 4;
  } else if (sideX == -1 && sideY == 0 && sideZ == 0) {
    side = 5;
  }

  // Serial.println(side);
}

// TODO replace with udp
void MpuHelperClass::sendHttpRequest() {
  if (prevSide == side) {
    return;
  }
  unsigned long httpStartTime = millis();

  HTTPClient http;
  Serial.println(urls[side]);

  http.begin(urls[side]);
  http.GET();
  http.end();

  httpTime = millis() - httpStartTime;
  Serial.print("Setup MPU helper took ");
  Serial.println(setupTime);
}

void MpuHelperClass::logSettings() {
  Serial.print("WakeCycle ");
  Serial.println(String(mpu6050.getWakeCycleEnabled()));

  Serial.print("DHPFMode ");
  Serial.println(String(mpu6050.getDHPFMode()));
  Serial.print("DLPFMode ");
  Serial.println(String(mpu6050.getDLPFMode()));

  Serial.println("AccelStandby ");
  Serial.println(String(mpu6050.getStandbyXAccelEnabled()));

  Serial.print("GyroStandby ");
  Serial.println(String(mpu6050.getStandbyXGyroEnabled()));
  Serial.print("TempSensor ");
  Serial.println(String(mpu6050.getTempSensorEnabled()));
  Serial.println();

  Serial.print("FIFO Enabled ");
  Serial.println(String(mpu6050.getFIFOEnabled()));
  Serial.print("XGyroFifo Enabled ");
  Serial.println(String(mpu6050.getXGyroFIFOEnabled()));
  Serial.print("IntFIFOBufferOverflow Enabled ");
  Serial.println(String(mpu6050.getIntFIFOBufferOverflowEnabled()));
  Serial.print("Clock Source ");
  Serial.println(String(mpu6050.getClockSource()));
  Serial.print("FullScalAccelRange ");
  Serial.println(String(mpu6050.getFullScaleAccelRange()));
  Serial.print("FullScalGyroRange ");
  Serial.println(String(mpu6050.getFullScaleGyroRange()));

  Serial.println();
}

void MpuHelperClass::server() {
  Serial.println("Setup MPU server");

  WebServerHelper.server.on("/api/mpu", HTTP_GET, [this](AsyncWebServerRequest *request) {
    int args = request->args();

    if (args > 0) {
      request->send(200, "text/plain", "message received");
      Serial.println("Update mpu settings");

      for (int i = 0; i < 6; i++) {
        String value = request->arg("side" + String(i));
        if (value.length() > 0) {
          urls[i] = request->arg(side);
        }
      }

      if (request->hasArg("duration")) duration = request->arg("duration").toInt();
      if (request->hasArg("threshold")) threshold = request->arg("threshold").toInt();

      write();

    } else {
      AsyncJsonResponse *response = new AsyncJsonResponse();
      response->addHeader("Server", "ESP Async Web Server");
      JsonVariant &root = response->getRoot();

      root["side"] = side;
      root["setupTime"] = setupTime;
      root["httpTime"] = httpTime;
      root["duration"] = duration;
      root["threshold"] = threshold;

      for (int i = 0; i < 6; i++) {
        root["side" + String(i)] = urls[i];
      }

      response->setLength();
      request->send(response);
    }
  });
}

MpuHelperClass MpuHelper;
