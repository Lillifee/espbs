#include "UltrasonicHelper.h"

void UltrasonicHelperClass::setup()
{
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

float UltrasonicHelperClass::readDistance()
{
  float distance, duration;

  // Clear the trigPin by setting it LOW:
  digitalWrite(trigPin, LOW);

  delayMicroseconds(5);
  // Trigger the sensor by setting the trigPin high for 10 microseconds:
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Read the echoPin. pulseIn() returns the duration (length of the pulse) in microseconds:
  duration = pulseIn(echoPin, HIGH);

  // Calculate the distance:
  distance = duration * 0.034 / 2;

  if (distance > 0)
  {
    // Print the distance on the Serial Monitor (Ctrl+Shift+M):
    Serial.print("Distance = ");
    Serial.print(distance);
    Serial.println(" cm");
  }
  return (distance);
}

float UltrasonicHelperClass::readDistanceAverage()
{
  float sum = 0;
  
  for (int i = 0; i < averageRuns; i++)
  {
    float dist = readDistance();
    if (dist > 0)
    {
      sum += dist;
    }
  }

  float distance = sum / averageRuns;
  return distance;
}

char* UltrasonicHelperClass::convertToString(float distance) {
  char buffer[8];
  return dtostrf(distance, 1, 2, buffer);
}