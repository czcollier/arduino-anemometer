#ifndef SLIDING_WINDOW_AVERAGE_H
#define SLIDING_WINDOW_AVERAGE_H

class SlidingWindowAverage {
public:
  // Constructor: Specifies the size of the sliding window
  SlidingWindowAverage(int windowSize);

  // Destructor: Cleans up memory
  ~SlidingWindowAverage();

  // Add a new value to the window
  void addValue(float newValue);

  // Get the current average
  float getAverage();

  // Clears the history
  void clear();

private:
  int size;
  float* readings;
  int currentIndex;
  float currentSum;
  bool windowIsFull;
};

#endif