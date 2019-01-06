
#ifndef Configurator_h
#define Configurator_h

#include <FS.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <WiFiManager.h>
#include <ArduinoJson.h>
#include <ESP8266WebServer.h>
#include <ESP8266TrueRandom.h>
#include "SimpleList.h"

/*
  Configurator is an opinionated high level class that contains functionality for
  reading & writing persistent configuration to the filesystem. When it
  can't obtain config values (ex. when it runs for the first time or is reset)
  it starts up an access point and a web server to facilitate human input.
*/

struct ConfigOption {
  String id;
  String value;
};

class Configurator {
  public:
    ~Configurator();

    static Configurator* Instance();

    WiFiManager wifiManager;
    WiFiServer wifiServer;

    void setup(int buttonId);
    void loop();
    void save();
    void reset();
    void addConfigOption(
      const char* id,
      const char* placeholder,
      const char* defaultValue,
      int length
    );
    const char* getConfigValue(const char* id);
    bool hasConfigOption(const char* id);

  private:
    Configurator();
    static Configurator* instance;
    int buttonId;

    SimpleList<WiFiManagerParameter*> configParams;
    SimpleList<ConfigOption*> configOptions;

    void readConfigFile();
    void writeConfigFile();
    void updateConfigOptionFromParams();
    void clearConfigOptions();
};

#endif
