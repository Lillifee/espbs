#include "MpuHelper.h"

const String urlPart1 = "http://webserver/dev/sps/io/ESP1-";
const String urlPart2 = "/pulse";

void MpuHelperClass::setup() {
  Wire.begin(SDA, SCL);

  // TODO remove afterwards
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
  mpu6050.setMotionDetectionDuration(1);
  mpu6050.setMotionDetectionThreshold(20);

  // logSettings();
}

void MpuHelperClass::loop() {
  readValues();
  sendHttpRequest();
}

void MpuHelperClass::sleep() {
  // set accel HPF to hold
  mpu6050.setDHPFMode(7);

  // set the frequency to wakeup
  mpu6050.setWakeFrequency(0);

  mpu6050.setStandbyXGyroEnabled(true);  // should be already set
  mpu6050.setStandbyYGyroEnabled(true);  // should be already set
  mpu6050.setStandbyZGyroEnabled(true);  // should be already set

  mpu6050.setTempSensorEnabled(false);
  mpu6050.setWakeCycleEnabled(true);

  // mpu6050.setFullScaleAccelRange(3);

  // Write low to MPU pins to fallback to 6.5uA
  digitalWrite(SDA, LOW);
  digitalWrite(SCL, LOW);
}

void MpuHelperClass::readValues() {
  mpu6050.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  Serial.print("a/g:\t");
  Serial.print(ax);
  Serial.print("\t");
  Serial.print(ay);
  Serial.print("\t");
  Serial.println(az);
  Serial.print("\t");

  calculateSide();
  sendHttpRequest();
}

int MpuHelperClass::calculateAxis(int16_t value) {
  return value > 8000 ? 1 : value < -8000 ? -1 : 0;
}

void MpuHelperClass::calculateSide() {
  int sideX = calculateAxis(ax);
  int sideY = calculateAxis(ay);
  int sideZ = calculateAxis(az);

  prevSide = side;

  if (sideX == 0 && sideY == 0 && sideZ == 1)
    side = "A";
  if (sideX == 0 && sideY == 0 && sideZ == -1)
    side = "B";
  if (sideX == 0 && sideY == -1 && sideZ == 0)
    side = "C";
  if (sideX == 0 && sideY == 1 && sideZ == 0)
    side = "D";
  if (sideX == 1 && sideY == 0 && sideZ == 0)
    side = "E";
  if (sideX == -1 && sideY == 0 && sideZ == 0)
    side = "F";

  // Serial.println(side);
}

// TODO replace with udp
void MpuHelperClass::sendHttpRequest() {
  if (MpuHelper.prevSide == MpuHelper.side)
    return;

  // This will send the request to the server
  HTTPClient http;
  String url = urlPart1 + MpuHelper.side + urlPart2;

  Serial.println(url);

  http.begin(url);
  http.GET();
  http.end();
}

void MpuHelperClass::logSettings() {
  Serial.println("WakeCycle" + String(mpu6050.getWakeCycleEnabled()));
  Serial.println("DHPFMode " + String(mpu6050.getDHPFMode()));
  Serial.println("DLPFMode " + String(mpu6050.getDLPFMode()));
  Serial.println();

  Serial.println("AccelStandby " + String(mpu6050.getStandbyXAccelEnabled()));
  Serial.println("GyroStandby " + String(mpu6050.getStandbyXGyroEnabled()));
  Serial.println("TempSensor  " + String(mpu6050.getTempSensorEnabled()));
  Serial.println();

  Serial.println("FIFO Enabled " + String(mpu6050.getFIFOEnabled()));
  Serial.println("XGyroFifo Enabled " + String(mpu6050.getXGyroFIFOEnabled()));
  Serial.println("IntFIFOBufferOverflow Enabled " + String(mpu6050.getIntFIFOBufferOverflowEnabled()));
  Serial.println("Clock Source " + String(mpu6050.getClockSource()));
  Serial.println("FullScalAccelRange " + String(mpu6050.getFullScaleAccelRange()));
  Serial.println("FullScalGyroRange " + String(mpu6050.getFullScaleGyroRange()));

  Serial.println();
}

MpuHelperClass MpuHelper;
