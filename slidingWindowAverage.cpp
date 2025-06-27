#include "slidingWindowAverage.h"

// Constructor implementation
SlidingWindowAverage::SlidingWindowAverage(int windowSize) {
  this->size = windowSize;
  this->readings = new float[windowSize];
  this->clear();
}

// Destructor implementation
SlidingWindowAverage::~SlidingWindowAverage() {
  delete[] this->readings;
}

void SlidingWindowAverage::clear() {
  this->currentIndex = 0;
  this->currentSum = 0;
  this->windowIsFull = false;
  for (int i = 0; i < this->size; i++) {
    this->readings[i] = 0.0;
  }
}

void SlidingWindowAverage::addValue(float newValue) {
  this->currentSum -= this->readings[this->currentIndex];
  this->readings[this->currentIndex] = newValue;
  this->currentSum += newValue;

  this->currentIndex++;
  if (this->currentIndex >= this->size) {
    this->currentIndex = 0;
    this->windowIsFull = true;
  }
}

float SlidingWindowAverage::getAverage() {
  if (this->windowIsFull) {
    return this->currentSum / this->size;
  } else {
    if (this->currentIndex == 0) {
      return 0.0;
    }
    return this->currentSum / this->currentIndex;
  }
}