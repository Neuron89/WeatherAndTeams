#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include "weather.h"
#include "calendar.h"

// Display dimensions for Waveshare 7.5inch E-Paper HAT (H)
// Resolution: 880×528 pixels
#define DISPLAY_WIDTH 880
#define DISPLAY_HEIGHT 528
#define SPLIT_POSITION (DISPLAY_WIDTH / 2)

// Function declarations
void initDisplay();
void displayStartupScreen();
void displayWiFiSetupScreen();
void drawSplitScreenLayout();
void drawWeatherData(const WeatherData &currentWeather, const HourlyForecast hourlyForecast[]);
void drawCalendarEvents(const CalendarEvents &events);
void drawWeatherIcon(int x, int y, int size, const String &iconCode);
void drawBatteryStatus(int x, int y);

#endif // DISPLAY_H
