#include "SplitFlapUnit.h"

SplitFlapUnit::SplitFlapUnit() {}

void SplitFlapUnit::setup() {
  // load the settings
  load();

  setMaxSpeed(1200);
  setAcceleration(1600);

  displayPos = 1;
  targetPos = 0;
  calcInterval();

  // initialize the pins
  pinMode(stepPin, OUTPUT);
  pinMode(hallPin, INPUT);

  Serial.print(index);
  Serial.print(" Setup stepPin ");
  Serial.print(stepPin);
  Serial.print(" hallPin");
  Serial.println(hallPin);
}

bool SplitFlapUnit::checkHall() {
  // Check the hall status and reset the position
  hallStatus = digitalRead(hallPin);

  // If the hall sensor switch from high to low (falling edge)
  bool hallDetected = ((hallStatusPrev == HIGH) & (hallStatus == LOW));

  hallStatusPrev = hallStatus;

  // if(fallingEdge) {
  // int offset = displayPos < FLAP_COUNT * STEPS_PER_FLAP
  //                  ? FLAP_COUNT * STEPS_PER_FLAP - displayPos
  //                  : displayPos;
  // Serial.print(index);
  // Serial.print(" - Hall sensor detected ");
  // Serial.print(displayPos);
  // Serial.print(" - steps away from the center. ");
  // Serial.println(offset);
  // }

  return hallDetected;
}

void SplitFlapUnit::load() {
  String indexString = String(index);
  preferences.begin((SPLIT_FLAP_UNIT + indexString).c_str(), true);

  stepPin = preferences.getInt("stepPin", 0);
  hallPin = preferences.getInt("hallPin", 0);
  hallOffset = preferences.getInt("offset", 0);

  preferences.end();
}

void SplitFlapUnit::save() {
  String indexString = String(index);
  preferences.begin((SPLIT_FLAP_UNIT + indexString).c_str(), false);

  preferences.putInt("stepPin", stepPin);
  preferences.putInt("hallPin", hallPin);
  preferences.putInt("offset", hallOffset);

  preferences.end();
}

void SplitFlapUnit::calibrate() {
  // TODO step until the hall sensor detected.
}

void SplitFlapUnit::setChar(char character) {
  if (targetChar == character) return;
  targetChar = character;

  // Find the index and the position of the char on change
  int targetIndex = FLAP_LETTERS.indexOf(targetChar);
  targetIndex = targetIndex >= 0 && targetIndex < FLAP_LETTERS.length() ? targetIndex : 0;
  targetPos = targetIndex * STEPS_PER_FLAP;

  // Serial.print(index);
  // Serial.print(" - Target ");
  // Serial.print(targetChar);
  // Serial.print(" at position ");
  // Serial.println(targetPos);
  calcInterval();
}

void SplitFlapUnit::loop() {
  if (!stepInterval) return;
  unsigned long time = micros();

  if (time - lastStepTime >= stepInterval) {
    step();

    // endless running
    // if (targetPos == displayPos) {
    //   targetPos = displayPos - 1;
    //   Serial.println("Target reachted");
    // }

    calcInterval();
    lastStepTime = time;
  }
}

bool SplitFlapUnit::running() {
  return stepInterval != 0;
}

void SplitFlapUnit::setAcceleration(float value) {
  initInterval = 0.676 * sqrt(2.0 / value) * 1000000.0;
  acceleration = value;
}

void SplitFlapUnit::setMaxSpeed(float value) {
  minInterval = 1000000.0 / value;
}

void SplitFlapUnit::calcInterval() {
  int distance = targetPos < displayPos ? (targetPos + FLAP_COUNT * STEPS_PER_FLAP) - displayPos
                                        : targetPos - displayPos;

  long stepsToStop = (long)((speed * speed) / (2.0 * acceleration));

  if (distance == 0 && stepsToStop <= DECELARATION_TOLERANCE) {
    stepInterval = 0;
    stepCounter = 0;
    speed = 0.0;
    return;
  }

  // start to decelarate
  if (stepsToStop >= distance) {
    // Only decelerate if we are in the tolerance to stop.
    if (stepsToStop - distance <= DECELARATION_TOLERANCE) {
      stepCounter = -stepsToStop;
    }
  }

  interval = stepCounter == 0 ? initInterval : interval - ((2.0 * interval) / ((4.0 * stepCounter) + 1));
  interval = max(interval, minInterval);

  stepCounter++;
  stepInterval = interval;
  speed = 1000000.0 / interval;
}

void SplitFlapUnit::step() {
  displayPos++;
  digitalWrite(stepPin, HIGH);
  delayMicroseconds(10);

  // Check the hall status and reset the position
  if (checkHall()) {
    displayPos = hallOffset;
  }

  // Reset the display position
  if (displayPos >= FLAP_COUNT * STEPS_PER_FLAP) {
    displayPos = 0;
    // Serial.print(index);
    // Serial.println(" - Reset pos to 0");
  }

  digitalWrite(stepPin, LOW);
}