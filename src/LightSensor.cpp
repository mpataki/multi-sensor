#include "LightSensor.h"

LightSensor::LightSensor(uint8_t pin) {
  this->pin = pin;
}

LightSensor::~LightSensor() {}

void LightSensor::setup() {
  pinMode(pin, INPUT);
}

float LightSensor::getLightReading() {
  // divide by max value for result in rand 0 -> 1
  return analogRead(pin) / 1023.0;
}
