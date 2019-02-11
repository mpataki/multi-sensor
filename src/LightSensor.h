#ifndef LightSensor_h
#define LightSensor_h

#include <Arduino.h>

class LightSensor {
public:
  LightSensor(uint8_t pin);
  ~LightSensor();

  void setup();
  float getLightReading();
private:
  uint8_t pin;
};

#endif
