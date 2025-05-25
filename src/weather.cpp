#include "weather.h"
#include "config.h"
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <WiFi.h>

// OpenWeatherMap API endpoint
const char* WEATHER_API_ENDPOINT = "https://api.openweathermap.org/data/2.5/onecall";
// IP Geolocation API endpoint
const char* IP_GEOLOCATION_API = "http://ip-api.com/json/";

// Get location information based on IP address
bool getLocationFromIP(String &city, String &country) {
  HTTPClient http;
  http.begin(IP_GEOLOCATION_API);
  
  int httpCode = http.GET();
  if (httpCode != HTTP_CODE_OK) {
    Serial.printf("IP Geolocation API failed, error: %d\n", httpCode);
    http.end();
    return false;
  }
  
  String payload = http.getString();
  http.end();
  
  // Parse JSON response
  DynamicJsonDocument doc(1024);
  DeserializationError error = deserializeJson(doc, payload);
  
  if (error) {
    Serial.print("JSON parsing failed: ");
    Serial.println(error.c_str());
    return false;
  }
  
  // Extract location data
  if (doc["status"] == "success") {
    city = doc["city"].as<String>();
    country = doc["country"].as<String>();
    Serial.printf("Location detected: %s, %s\n", city.c_str(), country.c_str());
    return true;
  }
  
  return false;
}

// Get weather data from OpenWeatherMap API
bool getWeatherData(WeatherData &currentWeather, HourlyForecast hourlyForecast[]) {
  // Access the global config variable
  extern Config config;
  
  // Get location from IP if not set in config
  String city, country;
  if (config.location.isEmpty()) {
    if (!getLocationFromIP(city, country)) {
      Serial.println("Failed to get location from IP");
      return false;
    }
    currentWeather.location = city + ", " + country;
  } else {
    currentWeather.location = config.location;
  }
  
  // Prepare API request
  String url = String(WEATHER_API_ENDPOINT);
  url += "?lat=" + String(0.0); // Replace with actual coordinates from geolocation
  url += "&lon=" + String(0.0); // Replace with actual coordinates from geolocation
  url += "&exclude=minutely,daily,alerts";
  url += "&units=metric";
  url += "&appid=YOUR_API_KEY"; // Replace with your OpenWeatherMap API key
  
  HTTPClient http;
  http.begin(url);
  
  int httpCode = http.GET();
  if (httpCode != HTTP_CODE_OK) {
    Serial.printf("Weather API request failed, error: %d\n", httpCode);
    http.end();
    return false;
  }
  
  String payload = http.getString();
  http.end();
  
  // Parse JSON response
  DynamicJsonDocument doc(16384); // Larger buffer for the complete weather data
  DeserializationError error = deserializeJson(doc, payload);
  
  if (error) {
    Serial.print("Weather JSON parsing failed: ");
    Serial.println(error.c_str());
    return false;
  }
  
  // Extract current weather data
  JsonObject current = doc["current"];
  currentWeather.timestamp = current["dt"].as<long>();
  currentWeather.temperature = current["temp"].as<float>();
  currentWeather.feelsLike = current["feels_like"].as<float>();
  currentWeather.humidity = current["humidity"].as<int>();
  currentWeather.pressure = current["pressure"].as<int>();
  currentWeather.windSpeed = current["wind_speed"].as<float>();
  currentWeather.uvIndex = current["uvi"].as<int>();
  
  // Get weather description and icon
  if (current["weather"].size() > 0) {
    currentWeather.description = current["weather"][0]["description"].as<String>();
    currentWeather.iconCode = current["weather"][0]["icon"].as<String>();
  }
  
  // Extract hourly forecast (next 24 hours)
  JsonArray hourly = doc["hourly"];
  int count = min((int)hourly.size(), 24);
  
  for (int i = 0; i < count; i++) {
    hourlyForecast[i].timestamp = hourly[i]["dt"].as<long>();
    hourlyForecast[i].temperature = hourly[i]["temp"].as<float>();
    hourlyForecast[i].precipitation = hourly[i]["pop"].as<float>() * 100; // Probability of precipitation as percentage
    
    if (hourly[i]["weather"].size() > 0) {
      hourlyForecast[i].iconCode = hourly[i]["weather"][0]["icon"].as<String>();
    }
  }
  
  return true;
}
