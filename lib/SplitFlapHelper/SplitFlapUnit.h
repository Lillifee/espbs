
#ifndef SplitFlapUnit_h
#define SplitFlapUnit_h

#include "Arduino.h"
#include "Preferences.h"

static const int FLAP_COUNT = 48;
static const float STEPS_PER_FLAP = 42.666;  // full rotation (2038 steps) / flap count (2048 / 48) last measure ~ 2060
static const String FLAP_LETTERS = " ABCDEFGHIJKLMNOPQRSTUVWXYZ$&#0123456789:.-?!*()";

static const String SPLIT_FLAP_UNIT = "splitflap";

static const int DECELARATION_TOLERANCE = 20;

class SplitFlapUnit {
 private:
  void step();
  bool checkHall();

  Preferences preferences;
  unsigned long lastUpdate;

  int stepStatus;

  int hallStatusPrev;
  int hallStatus;

  int displayPos;
  int targetPos;

  char targetChar;

  int stepSpeed;
  int accelSteps;

  unsigned long stepInterval;
  unsigned long lastStepTime;

  float speed;
  float acceleration;

  void setAcceleration(float value);
  void setSpeed(float value);
  void setMaxSpeed(float value);
  void reset();
  void calcInterval();

  long stepCounter;
  float initInterval;
  float interval;
  float minInterval;  // at max speed

 public:
  SplitFlapUnit();
  void setup();
  void load();
  void save();
  void loop();
  void calibrate();
  void setChar(char character);
  int getStepInterval();
  bool running();

  int index;
  int stepPin;
  int hallPin;
  int hallOffset;
};

#endif