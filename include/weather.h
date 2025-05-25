#ifndef WEATHER_H
#define WEATHER_H

#include <Arduino.h>

// Weather data structures
struct WeatherData {
  String location;
  String description;
  String iconCode;
  float temperature;
  float feelsLike;
  int humidity;
  float windSpeed;
  String windDirection;
  int pressure;
  int uvIndex;
  time_t timestamp;
};

struct HourlyForecast {
  time_t timestamp;
  String iconCode;
  float temperature;
  int precipitation;
};

// Function declarations
bool getWeatherData(WeatherData &currentWeather, HourlyForecast hourlyForecast[]);
bool getLocationFromIP(String &city, String &country);

#endif // WEATHER_H
