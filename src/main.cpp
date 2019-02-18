#include <PubSubClient.h>
#include "Configurator.h"
#include "ClimateSensor.h"
#include "LightSensor.h"
#include "MotionSensor.h"

#define MQTT_SERVER "mqtt_server"
#define MQTT_PORT "mqtt_port"
#define MQTT_CLIENT_ID "mqtt_client_id"
#define MQTT_USERNAME "mqtt_username"
#define MQTT_PASSWORD "mqtt_password"
#define MQTT_TOPIC "mqtt_topic"
#define CORE_LOOP_DELAY "core_loop_delay"
#define CLIMATE_SENSOR_PIN 5
#define LIGHT_SENSOR_PIN A0
#define MOTION_SENSOR_PIN 14
#define CONFIG_RESET_PIN 0

ClimateSensor climateSensor(CLIMATE_SENSOR_PIN);
LightSensor lightSensor(LIGHT_SENSOR_PIN);
MotionSensor motionSensor(MOTION_SENSOR_PIN);
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

void sendStateUpdate() {
  StaticJsonBuffer<256> jsonBuffer;

  JsonObject& root = jsonBuffer.createObject();
  root["temperature"] = climateSensor.getTemperature();
  root["humidity"] = climateSensor.getHumidity();
  root["light"] = lightSensor.getLightReading();
  root["motion"] = motionSensor.getSensorValue();

  char payload[256];
  root.printTo(payload, sizeof(payload));

  root.prettyPrintTo(Serial);
  mqttClient.publish(Configurator::Instance()->getConfigValue(MQTT_TOPIC), payload);
}

bool ensureMqttConnected() {
  if (mqttClient.connected()) return true;

  int retryLimit = 5;
  int tries = 1;

  while (!mqttClient.connected()) {
    Serial.println("Attempting MQTT connection...");

    bool successfulConnection = mqttClient.connect(
      Configurator::Instance()->getConfigValue(MQTT_CLIENT_ID),
      Configurator::Instance()->getConfigValue(MQTT_USERNAME),
      Configurator::Instance()->getConfigValue(MQTT_PASSWORD)
    );

    if (successfulConnection) {
      Serial.println("MQTT connection established");
    } else if (tries < retryLimit) {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(", retrying in 5 seconds");

      delay(5000);
      tries++;
    } else {
      return false;
    }
  }

  return true;
}

void setup()
{
  Serial.begin(9600);
  Serial.println("Starting up module");

  // Configurator::Instance()->reset(); // testing

  String defaultClientId = "ESP-" + String(ESP.getChipId());
  String defaultTopic = "climate/ESP-" + String(ESP.getChipId());
  Serial.println("My name is " + defaultClientId);

  Configurator::Instance()->addConfigOption(MQTT_SERVER, "MQTT server", "", 40);
  Configurator::Instance()->addConfigOption(MQTT_PORT, "MQTT port", "1883", 10);
  Configurator::Instance()->addConfigOption(MQTT_CLIENT_ID, "MQTT Client ID", defaultClientId.c_str(), 20);
  Configurator::Instance()->addConfigOption(MQTT_USERNAME, "MQTT Username", "", 20);
  Configurator::Instance()->addConfigOption(MQTT_PASSWORD, "MQTT Password", "", 40);
  Configurator::Instance()->addConfigOption(MQTT_TOPIC, "MQTT Topic", defaultTopic.c_str(), 40);
  Configurator::Instance()->addConfigOption(CORE_LOOP_DELAY, "Core Loop Delay", "60000", 6);

  Configurator::Instance()->setup(CONFIG_RESET_PIN);
  climateSensor.setup();
  lightSensor.setup();
  motionSensor.setup();

  const char* mqttServerAddress = Configurator::Instance()->getConfigValue(MQTT_SERVER);
  const char* mqttServerPort = Configurator::Instance()->getConfigValue(MQTT_PORT);

  Serial.println("Setting MQTT server addres to " + String(mqttServerAddress) + ", port " + String(mqttServerPort));
  mqttClient.setServer(mqttServerAddress, atoi(mqttServerPort));
}

void loop()
{
  Configurator::Instance()->loop();

  ensureMqttConnected();

  sendStateUpdate();
  mqttClient.loop();

  // should use delay vs some sort of ESP level sleep to reduce power consumption?
  delay(max(
    (uint32_t)atoi(Configurator::Instance()->getConfigValue("core_loop_delay")),
    climateSensor.getMinSensorScanInterval()
  ));
}
