// FrequencyCounter.h

#ifndef FREQUENCY_COUNTER_H
#define FREQUENCY_COUNTER_H

#include <Arduino.h> // Needed for unsigned long, etc.

class FrequencyCounter {
public:
  // Constructor: Sets up the object with our parameters
  FrequencyCounter(int high, int low, unsigned long timeout_ms);

  // The only input: feed the class a new sensor reading
  void update(unsigned long currentMillis, int sensorValue);

  // The only output: safely get the current frequency
  float getFrequency();

private:
  // All internal state is private and protected
  int highThreshold;
  int lowThreshold;
  unsigned long timeoutMs; // Store timeout in microseconds for precision

  bool isArmed;
  bool hasStarted;
  unsigned long lastEventTime;
  float currentFrequency;
};

#endif