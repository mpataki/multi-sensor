#include "Configurator.h"

// WiFiManager's requirement for this function to be global is
//  the only reason to use the Singleton pattern here.
void saveConfigCallback () {
  Serial.println("WiFiManager's saveConfigCallback called");
  Configurator::Instance()->save();
}

Configurator* Configurator::instance = NULL;

Configurator* Configurator::Instance() {
  if (instance == NULL)
    instance = new Configurator();

  return instance;
}

Configurator::Configurator() :
  wifiServer(80),
  configParams(),
  configOptions()
{
  wifiManager.setSaveConfigCallback(saveConfigCallback);

  // AP timeout
  wifiManager.setTimeout(120);
}

Configurator::~Configurator() {
  SimpleList<WiFiManagerParameter*>::iterator iter;
  for (iter=configParams.begin(); iter != configParams.end(); iter++)
    delete iter;

  clearConfigOptions();
}

void Configurator::save() {
  updateConfigOptionFromParams();
  writeConfigFile();
}

void Configurator::reset() {
  wifiManager.resetSettings();
  SPIFFS.format();
}

void Configurator::clearConfigOptions() {
  SimpleList<ConfigOption*>::iterator iter;

  for (iter=configOptions.begin(); iter != configOptions.end(); ++iter)
    delete *iter;

  configOptions.clear();
}

void Configurator::setup(int buttonId) {
  this->buttonId = buttonId;

  pinMode(buttonId, INPUT);

  readConfigFile();

  if (!wifiManager.autoConnect()) { // use password here?
    Serial.println("Failed to connect to wifi. Trying again shortly.");

    // random delay to stagger reconnects from multiple devices
    //   when the wifi goes down.
    delay(3000 + ESP8266TrueRandom.random(4000));
    ESP.reset();
  }

  Serial.println("Connected to wifi");
}

void Configurator::loop() {
  if (digitalRead(buttonId) == HIGH) return;

  Serial.println("button pressed - resetting config and restarting device");
  reset();
  ESP.reset();
}

void Configurator::addConfigOption(
  const char* id,
  const char* placeholder,
  const char* defaultValue,
  int length)
{
  if (hasConfigOption(id)) return;

  WiFiManagerParameter* param =
    new WiFiManagerParameter(id, placeholder, defaultValue, length);

  wifiManager.addParameter(param);
  configParams.push_back(param);
}

const char* Configurator::getConfigValue(const char* id) {
  SimpleList<ConfigOption*>::iterator iter;
  ConfigOption* option;

  for (iter=configOptions.begin(); iter != configOptions.end(); iter++) {
    option = *iter;
    if (option->id.equals(id))
      return option->value.c_str();
  }

  Serial.println("Couldn't find config value for " + String(id));
  return NULL;
}

bool Configurator::hasConfigOption(const char* id) {
  SimpleList<WiFiManagerParameter*>::iterator iter;

  for (iter=configParams.begin(); iter != configParams.end(); iter++)
    if (strcmp(id, (*iter)->getID()) == 0)
      return true;

  return false;
}

void Configurator::updateConfigOptionFromParams() {
  clearConfigOptions();

  SimpleList<WiFiManagerParameter*>::iterator iter;
  WiFiManagerParameter* param;
  for (iter=configParams.begin(); iter != configParams.end(); iter++) {
    param = *iter;

    ConfigOption* option = new ConfigOption;
    option->id = String(param->getID());
    option->value = String(param->getValue());

    Serial.println("updated config option: " + option->id + " : " + option->value);

    configOptions.push_back(option);
  }
}

void Configurator::readConfigFile() {
  Serial.println("mounting file system...");

  if (!SPIFFS.begin()) {
    Serial.println("failed to mount file system");
    return;
  }

  Serial.println("mounted file system");

  if (!SPIFFS.exists("/config.json")) {
    Serial.println("config.json doesn't exist");
    return;
  }

  Serial.println("reading config file");
  File configFile = SPIFFS.open("/config.json", "r");

  if (!configFile) {
    Serial.println("couldn't read config.json");
    return;
  }

  Serial.println("opened config file");
  size_t size = configFile.size();
  // Allocate a buffer to store contents of the file.
  std::unique_ptr<char[]> buf(new char[size]);

  configFile.readBytes(buf.get(), size);
  DynamicJsonBuffer jsonBuffer;
  JsonObject& json = jsonBuffer.parseObject(buf.get());
  json.printTo(Serial);
  Serial.println("");

  if (json.success()) {
    Serial.println("parsed json");

    clearConfigOptions();

    JsonObject::iterator iter;
    for (iter=json.begin(); iter != json.end(); ++iter) {
      ConfigOption* option = new ConfigOption();
      option->id = String(iter->key);
      option->value = String(iter->value.as<char*>());

      configOptions.push_back(option);
      Serial.println("updated config option: " + option->id + " : " + option->value);
    }
  } else {
    Serial.println("\nfailed to load json config");
  }

  configFile.close();
}

void Configurator::writeConfigFile() {
  Serial.println("writing config");

  DynamicJsonBuffer jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();

  SimpleList<WiFiManagerParameter*>::iterator iter; // TODO: iterate over values
  WiFiManagerParameter* param;

  for (iter=configParams.begin(); iter != configParams.end(); iter++) {
    param = *iter;
    json[param->getID()] = param->getValue();
  }

  File configFile = SPIFFS.open("/config.json", "w");

  if (configFile) {
    json.printTo(configFile);
  } else {
    Serial.println("failed to open config file for writing");
  }

  json.printTo(Serial);
  Serial.println("");
  configFile.close();
}
