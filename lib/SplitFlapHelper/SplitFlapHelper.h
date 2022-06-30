
#ifndef SplitFlapHelper_h
#define SplitFlapHelper_h

#include "Arduino.h"
#include "Preferences.h"
#include "SplitFlapUnit.h"
#include "WebServerHelper.h"

static const int MAX_UNIT_COUNT = 12;
static const int enablePin = 23;

class SplitFlapHelperClass {
 private:
  SemaphoreHandle_t prefMutex = NULL;

  Preferences preferences;
  SplitFlapUnit units[MAX_UNIT_COUNT];

  void load();
  void save();

  void setText();
  void calibrate();
  void loopUnits();
  void enableUnits(bool enable);

  int mode;
  String targetText;
  bool enabled;
  unsigned long setupDuration;

 public:
  SplitFlapHelperClass();

  void setup();
  void server();
  void loop();

  String text;
};

extern SplitFlapHelperClass SplitFlapHelper;

#endif