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

// Load configuration from non-volatile storage
bool loadConfig() {
  extern Config config;
  extern Preferences preferences;
  
  preferences.begin("eink-weather", true); // Read-only mode
  
  config.location = preferences.getString("location", "");
  config.weatherApiKey = preferences.getString("weather_key", "");
  config.msftClientId = preferences.getString("msft_id", "");
  config.msftClientSecret = preferences.getString("msft_secret", "");
  config.msftRefreshToken = preferences.getString("msft_token", "");
  
  preferences.end();
  
  return true;
}

// Save configuration to non-volatile storage
bool saveConfig() {
  extern Config config;
  extern Preferences preferences;
  
  preferences.begin("eink-weather", false); // Read-write mode
  
  preferences.putString("location", config.location);
  preferences.putString("weather_key", config.weatherApiKey);
  preferences.putString("msft_id", config.msftClientId);
  preferences.putString("msft_secret", config.msftClientSecret);
  preferences.putString("msft_token", config.msftRefreshToken);
  
  preferences.end();
  
  return true;
}

#endif // CONFIG_H
