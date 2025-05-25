#include <Arduino.h>
#include <WiFi.h>
#include <WiFiManager.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <TimeLib.h>
#include "GxEPD2_BW.h"
#include "GxEPD2_3C.h"
#include "epd_driver.h"
#include "weather.h"
#include "calendar.h"
#include "display.h"
#include "config.h"

// Display instance for Waveshare 7.5inch E-Paper HAT (H)
// Using GxEPD2_BW class for black and white display
GxEPD2_BW<GxEPD2_750_T7, GxEPD2_750_T7::HEIGHT> display(GxEPD2_750_T7(/*CS=*/ 15, /*DC=*/ 27, /*RST=*/ 26, /*BUSY=*/ 25));

// Global variables
WeatherData currentWeather;
HourlyForecast hourlyForecast[24];
CalendarEvents calendarEvents;
Config config;

void setup() {
  Serial.begin(115200);
  Serial.println("E-Ink Weather and Calendar Display");

  // Initialize display
  initDisplay();
  
  // Show startup screen
  displayStartupScreen();
  
  // Initialize WiFi with captive portal
  setupWiFi();
  
  // Get time from NTP server
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");
  
  // Initial data fetch
  updateWeatherData();
  updateCalendarData();
  
  // Update display with fetched data
  updateDisplay();
}

void loop() {
  // Check if it's time to update data (every 30 minutes)
  static unsigned long lastUpdateTime = 0;
  if (millis() - lastUpdateTime > 30 * 60 * 1000 || lastUpdateTime == 0) {
    Serial.println("Updating data...");
    updateWeatherData();
    updateCalendarData();
    updateDisplay();
    lastUpdateTime = millis();
  }
  
  // Deep sleep to save power (wake up every 30 minutes)
  Serial.println("Going to deep sleep...");
  esp_sleep_enable_timer_wakeup(30 * 60 * 1000000); // 30 minutes in microseconds
  esp_deep_sleep_start();
}

void setupWiFi() {
  WiFiManager wifiManager;
  
  // Set custom parameters for the captive portal
  WiFiManagerParameter custom_location("location", "Location (optional)", "", 40);
  wifiManager.addParameter(&custom_location);
  
  // Set timeout for captive portal
  wifiManager.setConfigPortalTimeout(180);
  
  // Set custom AP name
  String apName = "EinkWeather_" + String((uint32_t)ESP.getEfuseMac(), HEX);
  
  // Start the captive portal
  Serial.println("Starting WiFi setup portal...");
  displayWiFiSetupScreen();
  
  if (!wifiManager.autoConnect(apName.c_str())) {
    Serial.println("Failed to connect and hit timeout");
    ESP.restart();
    delay(1000);
  }
  
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  // Save custom parameters
  config.location = String(custom_location.getValue());
  saveConfig();
}

void updateWeatherData() {
  if (getWeatherData(currentWeather, hourlyForecast)) {
    Serial.println("Weather data updated successfully");
  } else {
    Serial.println("Failed to update weather data");
  }
}

void updateCalendarData() {
  if (getCalendarEvents(calendarEvents)) {
    Serial.println("Calendar data updated successfully");
  } else {
    Serial.println("Failed to update calendar data");
  }
}

void updateDisplay() {
  // Clear display
  display.firstPage();
  do {
    // Draw split screen layout
    drawSplitScreenLayout();
    
    // Draw weather on left side
    drawWeatherData(currentWeather, hourlyForecast);
    
    // Draw calendar on right side
    drawCalendarEvents(calendarEvents);
    
  } while (display.nextPage());
}

void saveConfig() {
  // Save configuration to SPIFFS or preferences
  Serial.println("Saving configuration...");
}
