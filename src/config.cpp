#include "config.h"

// Preferences instance for storing configuration
Preferences preferences;

// Load configuration from non-volatile storage
bool loadConfig() {
  preferences.begin("eink-weather", true); // Read-only mode
  
  Config config;
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
  preferences.begin("eink-weather", false); // Read-write mode
  
  preferences.putString("location", config.location);
  preferences.putString("weather_key", config.weatherApiKey);
  preferences.putString("msft_id", config.msftClientId);
  preferences.putString("msft_secret", config.msftClientSecret);
  preferences.putString("msft_token", config.msftRefreshToken);
  
  preferences.end();
  
  return true;
}
