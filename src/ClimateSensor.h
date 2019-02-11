#ifndef ClimateSensor_h
#define ClimateSensor_h
#define DHTPIN 0       // Pin which is connected to the DHT sensor. // TODO: this should be passed as a variable to this class
#define DHTTYPE DHT22  // AM2302

#include <DHT.h>
#include <DHT_U.h>
#include <Wire.h>

class ClimateSensor {
public:
  ClimateSensor();
  ~ClimateSensor();

  void setup();
  float getTemperature();
  float getHumidity();
  uint32_t getMinSensorScanInterval();

private:
  DHT_Unified dht;
  uint32_t minSensorScanInterval; // ms
  float lastHumidity;
  float lastTemperature;
};

#endif
