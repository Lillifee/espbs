#ifndef UltrasonicHelper_h
#define UltrasonicHelper_h

#include "Arduino.h"

#define trigPin 35
#define echoPin 34

#define averageRuns 5

class UltrasonicHelperClass
{
private:
    long duartion;

public:
    void setup();
    float readDistance();
    float readDistanceAverage();
    char* convertToString(float);
};

extern UltrasonicHelperClass UltrasonicHelper;

#endif