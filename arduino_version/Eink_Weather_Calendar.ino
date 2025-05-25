/*
 * E-Ink Weather and Calendar Display
 * 
 * This sketch displays weather information and calendar events on a
 * Waveshare 7.5-inch E-Ink display connected to an ESP32.
 * 
 * Hardware:
 * - ESP32 Development Board
 * - Waveshare 7.5-inch E-Paper HAT (H)
 * 
 * Libraries required:
 * - GxEPD2 by Jean-Marc Zingg
 * - ArduinoJson by Benoit Blanchon
 * - WiFiManager by tzapu
 * - Time by Paul Stoffregen
 */

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiManager.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <TimeLib.h>
#include <GxEPD2_BW.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeMonoBold12pt7b.h>
#include <Fonts/FreeMonoBold18pt7b.h>
#include <Fonts/FreeMonoBold24pt7b.h>
#include "secrets.h"

// Display dimensions for Waveshare 7.5inch E-Paper HAT (H)
#define DISPLAY_WIDTH 880
#define DISPLAY_HEIGHT 528
#define SPLIT_POSITION (DISPLAY_WIDTH / 2)

// Display instance for Waveshare 7.5inch E-Paper HAT (H)
GxEPD2_BW<GxEPD2_750_T7, GxEPD2_750_T7::HEIGHT> display(GxEPD2_750_T7(/*CS=*/ 15, /*DC=*/ 27, /*RST=*/ 26, /*BUSY=*/ 25));

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

// Calendar data structures
struct CalendarEvent {
  String title;
  String location;
  time_t startTime;
  time_t endTime;
  bool isAllDay;
};

struct CalendarEvents {
  CalendarEvent events[10]; // Maximum 10 events
  int count;
  time_t lastUpdated;
};

// Configuration structure
struct Config {
  String location;
  String weatherApiKey;
  String msftClientId;
  String msftClientSecret;
  String msftRefreshToken;
};

// Global variables
WeatherData currentWeather;
HourlyForecast hourlyForecast[24];
CalendarEvents calendarEvents;
Config config;
Preferences preferences;

// Forward declarations
void updateWeatherData();
void updateCalendarData();

// Function declarations
void setupWiFi();
bool getWeatherData(WeatherData &currentWeather, HourlyForecast hourlyForecast[]);
bool getLocationFromIP(String &city, String &country);
bool getCalendarEvents(CalendarEvents &events);
void initDisplay();
void displayStartupScreen();
void displayWiFiSetupScreen();
void updateDisplay();
void drawSplitScreenLayout();
void drawWeatherData(const WeatherData &currentWeather, const HourlyForecast hourlyForecast[]);
void drawCalendarEvents(const CalendarEvents &events);
void drawWeatherIcon(int x, int y, int size, const String &iconCode);
void drawBatteryStatus(int x, int y);
bool loadConfig();
bool saveConfig();

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

// Initialize the E-Ink display
void initDisplay() {
  display.init();
  display.setRotation(3); // Landscape mode
  display.setTextColor(GxEPD_BLACK);
  display.setFullWindow();
}

// Display startup screen
void displayStartupScreen() {
  display.firstPage();
  do {
    display.fillScreen(GxEPD_WHITE);
    display.setFont(&FreeMonoBold18pt7b);
    display.setCursor(50, DISPLAY_HEIGHT / 2);
    display.print("E-Ink Weather & Calendar");
    display.setFont(&FreeMonoBold12pt7b);
    display.setCursor(50, DISPLAY_HEIGHT / 2 + 40);
    display.print("Starting up...");
  } while (display.nextPage());
}

// Display WiFi setup screen
void displayWiFiSetupScreen() {
  display.firstPage();
  do {
    display.fillScreen(GxEPD_WHITE);
    display.setFont(&FreeMonoBold18pt7b);
    display.setCursor(50, DISPLAY_HEIGHT / 2 - 50);
    display.print("WiFi Setup Mode");
    display.setFont(&FreeMonoBold12pt7b);
    display.setCursor(50, DISPLAY_HEIGHT / 2);
    display.print("Connect to WiFi network:");
    display.setCursor(50, DISPLAY_HEIGHT / 2 + 30);
    display.print("EinkWeather_XXXXXX");
    display.setCursor(50, DISPLAY_HEIGHT / 2 + 60);
    display.print("Then open: 192.168.4.1");
  } while (display.nextPage());
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

// Draw the split screen layout
void drawSplitScreenLayout() {
  display.fillScreen(GxEPD_WHITE);
  
  // Draw vertical divider line
  display.drawLine(SPLIT_POSITION, 0, SPLIT_POSITION, DISPLAY_HEIGHT, GxEPD_BLACK);
  
  // Draw headers
  display.setFont(&FreeMonoBold12pt7b);
  display.setCursor(10, 30);
  display.print("Weather");
  
  display.setCursor(SPLIT_POSITION + 10, 30);
  display.print("Calendar");
  
  // Draw horizontal lines under headers
  display.drawLine(10, 40, SPLIT_POSITION - 10, 40, GxEPD_BLACK);
  display.drawLine(SPLIT_POSITION + 10, 40, DISPLAY_WIDTH - 10, 40, GxEPD_BLACK);
  
  // Draw battery status in top right corner
  drawBatteryStatus(DISPLAY_WIDTH - 50, 25);
}

// Draw weather data on the left side of the screen
void drawWeatherData(const WeatherData &currentWeather, const HourlyForecast hourlyForecast[]) {
  // Draw location
  display.setFont(&FreeMonoBold12pt7b);
  display.setCursor(20, 70);
  display.print(currentWeather.location);
  
  // Draw current weather icon (large)
  drawWeatherIcon(80, 150, 100, currentWeather.iconCode);
  
  // Draw current temperature
  display.setFont(&FreeMonoBold24pt7b);
  display.setCursor(200, 150);
  display.print(String(currentWeather.temperature, 1));
  display.print(" C");
  
  // Draw current weather details
  display.setFont(&FreeMonoBold9pt7b);
  int yPos = 180;
  display.setCursor(200, yPos);
  display.print("Feels like: ");
  display.print(String(currentWeather.feelsLike, 1));
  display.print(" C");
  
  yPos += 25;
  display.setCursor(200, yPos);
  display.print("Humidity: ");
  display.print(currentWeather.humidity);
  display.print("%");
  
  yPos += 25;
  display.setCursor(200, yPos);
  display.print("Wind: ");
  display.print(String(currentWeather.windSpeed, 1));
  display.print(" m/s");
  
  // Draw hourly forecast
  display.setFont(&FreeMonoBold12pt7b);
  display.setCursor(20, 280);
  display.print("Hourly Forecast");
  
  // Draw horizontal line
  display.drawLine(20, 290, SPLIT_POSITION - 20, 290, GxEPD_BLACK);
  
  // Draw hourly forecast items
  int hourWidth = (SPLIT_POSITION - 40) / 6; // Show 6 hours
  for (int i = 0; i < 6; i++) {
    int x = 20 + i * hourWidth;
    
    // Draw time
    char timeStr[6];
    time_t forecastTime = hourlyForecast[i].timestamp;
    struct tm *timeinfo = localtime(&forecastTime);
    sprintf(timeStr, "%02d:00", timeinfo->tm_hour);
    
    display.setFont(&FreeMonoBold9pt7b);
    display.setCursor(x, 320);
    display.print(timeStr);
    
    // Draw icon
    drawWeatherIcon(x + hourWidth/2 - 15, 350, 30, hourlyForecast[i].iconCode);
    
    // Draw temperature
    display.setCursor(x, 400);
    display.print(String(hourlyForecast[i].temperature, 0));
    display.print("C");
    
    // Draw precipitation chance
    display.setCursor(x, 430);
    display.print(String(hourlyForecast[i].precipitation));
    display.print("%");
  }
}

// Draw calendar events on the right side of the screen
void drawCalendarEvents(const CalendarEvents &events) {
  // Get current time for highlighting today's events
  time_t now = time(nullptr);
  struct tm *timeinfo = localtime(&now);
  int currentDay = timeinfo->tm_mday;
  int currentMonth = timeinfo->tm_mon;
  int currentYear = timeinfo->tm_year;
  
  // Draw current date
  char dateStr[20];
  strftime(dateStr, sizeof(dateStr), "%a, %b %d, %Y", timeinfo);
  
  display.setFont(&FreeMonoBold12pt7b);
  display.setCursor(SPLIT_POSITION + 20, 70);
  display.print(dateStr);
  
  // Draw events
  int yPos = 100;
  display.setFont(&FreeMonoBold9pt7b);
  
  if (events.count == 0) {
    display.setCursor(SPLIT_POSITION + 20, yPos);
    display.print("No upcoming events");
  } else {
    for (int i = 0; i < events.count; i++) {
      const CalendarEvent &event = events.events[i];
      
      // Format event time
      char startTimeStr[20], endTimeStr[20];
      struct tm *startTime = localtime(&event.startTime);
      struct tm *endTime = localtime(&event.endTime);
      
      if (event.isAllDay) {
        strcpy(startTimeStr, "All day");
      } else {
        strftime(startTimeStr, sizeof(startTimeStr), "%H:%M", startTime);
        strftime(endTimeStr, sizeof(endTimeStr), "%H:%M", endTime);
      }
      
      // Check if event is today
      bool isToday = (startTime->tm_mday == currentDay && 
                      startTime->tm_mon == currentMonth && 
                      startTime->tm_year == currentYear);
      
      // Draw event time
      display.setCursor(SPLIT_POSITION + 20, yPos);
      if (event.isAllDay) {
        display.print(startTimeStr);
      } else {
        display.print(startTimeStr);
        display.print(" - ");
        display.print(endTimeStr);
      }
      
      // Draw event title
      yPos += 25;
      display.setCursor(SPLIT_POSITION + 30, yPos);
      
      // Highlight today's events
      if (isToday) {
        display.setTextColor(GxEPD_BLACK);
        display.fillRect(SPLIT_POSITION + 25, yPos - 15, DISPLAY_WIDTH - SPLIT_POSITION - 45, 20, GxEPD_BLACK);
        display.setTextColor(GxEPD_WHITE);
      }
      
      // Truncate title if too long
      String title = event.title;
      if (title.length() > 25) {
        title = title.substring(0, 22) + "...";
      }
      display.print(title);
      
      // Reset text color
      if (isToday) {
        display.setTextColor(GxEPD_BLACK);
      }
      
      // Draw event location if available
      if (event.location.length() > 0) {
        yPos += 20;
        display.setCursor(SPLIT_POSITION + 30, yPos);
        
        // Truncate location if too long
        String location = event.location;
        if (location.length() > 25) {
          location = location.substring(0, 22) + "...";
        }
        display.print(location);
      }
      
      // Add separator line
      yPos += 15;
      display.drawLine(SPLIT_POSITION + 20, yPos, DISPLAY_WIDTH - 20, yPos, GxEPD_BLACK);
      yPos += 20;
      
      // Check if we've reached the bottom of the display
      if (yPos > DISPLAY_HEIGHT - 30) {
        display.setCursor(SPLIT_POSITION + 20, yPos);
        display.print("+ more events");
        break;
      }
    }
  }
}

// Draw weather icon based on icon code
void drawWeatherIcon(int x, int y, int size, const String &iconCode) {
  // This is a simplified implementation
  // In a real application, you would load bitmap icons based on the icon code
  
  // For now, just draw a placeholder rectangle
  display.drawRect(x, y, size, size, GxEPD_BLACK);
  
  // Draw icon code in the center for debugging
  display.setFont(&FreeMonoBold9pt7b);
  display.setCursor(x + size/4, y + size/2);
  display.print(iconCode);
}

// Draw battery status indicator
void drawBatteryStatus(int x, int y) {
  // Read battery voltage (simplified)
  float batteryVoltage = 3.7; // Example value
  int batteryPercentage = 75; // Example value
  
  // Draw battery icon
  display.drawRect(x, y - 10, 30, 15, GxEPD_BLACK);
  display.fillRect(x + 30, y - 5, 3, 5, GxEPD_BLACK);
  
  // Fill battery based on percentage
  int fillWidth = (batteryPercentage * 26) / 100;
  display.fillRect(x + 2, y - 8, fillWidth, 11, GxEPD_BLACK);
  
  // Draw percentage text
  display.setFont(&FreeMonoBold9pt7b);
  display.setCursor(x - 40, y);
  display.print(String(batteryPercentage) + "%");
}

// Get location information based on IP address
bool getLocationFromIP(String &city, String &country) {
  HTTPClient http;
  http.begin("http://ip-api.com/json/");
  
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
  String url = "https://api.openweathermap.org/data/2.5/onecall";
  url += "?lat=" + String(0.0); // Replace with actual coordinates from geolocation
  url += "&lon=" + String(0.0); // Replace with actual coordinates from geolocation
  url += "&exclude=minutely,daily,alerts";
  url += "&units=metric";
  url += "&appid=" + String(OPENWEATHERMAP_API_KEY);
  
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

// Get calendar events from Microsoft Outlook
bool getCalendarEvents(CalendarEvents &events) {
  // This is a simplified implementation for testing
  // In a real application, you would implement OAuth authentication and API calls
  
  Serial.println("Calendar API request not fully implemented");
  Serial.println("In a real application, you would make an authenticated request to Microsoft Graph API");
  
  // For testing purposes, create some dummy events
  events.count = 0;
  
  // Add dummy events
  CalendarEvent event1;
  event1.title = "Team Meeting";
  event1.location = "Conference Room A";
  event1.startTime = time(nullptr) + 3600; // 1 hour from now
  event1.endTime = time(nullptr) + 7200;   // 2 hours from now
  event1.isAllDay = false;
  events.events[events.count++] = event1;
  
  CalendarEvent event2;
  event2.title = "Project Deadline";
  event2.location = "";
  event2.startTime = time(nullptr) + 86400; // Tomorrow
  event2.endTime = time(nullptr) + 86400;
  event2.isAllDay = true;
  events.events[events.count++] = event2;
  
  events.lastUpdated = time(nullptr);
  
  return true;
}

// Load configuration from non-volatile storage
bool loadConfig() {
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
  preferences.begin("eink-weather", false); // Read-write mode
  
  preferences.putString("location", config.location);
  preferences.putString("weather_key", config.weatherApiKey);
  preferences.putString("msft_id", config.msftClientId);
  preferences.putString("msft_secret", config.msftClientSecret);
  preferences.putString("msft_token", config.msftRefreshToken);
  
  preferences.end();
  
  return true;
}
