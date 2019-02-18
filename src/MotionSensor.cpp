#include "MotionSensor.h"

MotionSensor::MotionSensor(uint8_t pin) {
  this->pin = pin;
}

MotionSensor::~MotionSensor() {}

void MotionSensor::setup() {
  pinMode(pin, INPUT);
}

bool MotionSensor::getSensorValue() {
  return digitalRead(pin) == 1;
}
