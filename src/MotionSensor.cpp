#include "MotionSensor.h"

MotionSensor::MotionSensor(uint8_t pin) {
  this->pin = pin;
}

MotionSensor::~MotionSensor() {}

void MotionSensor::setup() {
  Serial.println("Setting motion pin to input: " + String(pin));
  pinMode(pin, INPUT);
}

bool MotionSensor::getSensorValue() {
  Serial.println("Reading pin " + String(pin));
  int pinValue = digitalRead(pin);
  Serial.println("Motion: " + String(pinValue));
  return pinValue == 1;
}
