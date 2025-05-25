#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <Preferences.h>

// Configuration structure
struct Config {
  String location;
  String weatherApiKey;
  String msftClientId;
  String msftClientSecret;
  String msftRefreshToken;
};

// Function declarations
bool loadConfig();
bool saveConfig();

#endif // CONFIG_H
