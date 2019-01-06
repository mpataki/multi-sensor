
#include "ClimateSensor.h"

ClimateSensor::ClimateSensor() :
  dht(DHTPIN, DHTTYPE)
{
  lastHumidity = 0.0f;
  lastTemperature = 0.0f;
}

ClimateSensor::~ClimateSensor() {}

void ClimateSensor::setup() {
  Serial.println("Initializing Sensor");
  dht.begin();

  Serial.println("Sensor Information:");
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.println("Temperature");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" *C");
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" *C");
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" *C");
  Serial.println("------------------------------------");

  // Print humidity sensor details.
  dht.humidity().getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.println("Humidity");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println("%");
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println("%");
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println("%");
  Serial.println("------------------------------------");

  // Set delay between sensor readings based on sensor details.
  minSensorScanInterval = sensor.min_delay / 1000; // ms
}

float ClimateSensor::getTemperature() {
  sensors_event_t event;
  dht.temperature().getEvent(&event);

  if (isnan(event.temperature)) {
    Serial.println("Error while reading temperature");
    return lastTemperature;
  }

  lastTemperature = event.temperature; // C
  return lastTemperature;
}

float ClimateSensor::getHumidity() {
  sensors_event_t event;
  dht.humidity().getEvent(&event);

  if (isnan(event.relative_humidity)) {
    Serial.println("Error while reading humidity");
    return lastHumidity;
  }

  lastHumidity = event.relative_humidity; // %
  return lastHumidity;
}

uint32_t ClimateSensor::getMinSensorScanInterval() {
  return minSensorScanInterval;
}
