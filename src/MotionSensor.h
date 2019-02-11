#ifndef MotionSensor_h
#define MotionSensor_h

#include <Arduino.h>

class MotionSensor {
public:
  MotionSensor(uint8_t pin);
  ~MotionSensor();

  void setup();
  bool getSensorValue();
private:
  uint8_t pin;
};

#endif
