#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include "weather.h"
#include "calendar.h"
#include <GxEPD2_BW.h>

// Display dimensions for Waveshare 7.5inch E-Paper HAT (H)
#define DISPLAY_WIDTH 880
#define DISPLAY_HEIGHT 528
#define SPLIT_POSITION (DISPLAY_WIDTH / 2)

// External reference to display instance
extern GxEPD2_BW<GxEPD2_750_T7, GxEPD2_750_T7::HEIGHT> display;

// Function declarations
void initDisplay();
void displayStartupScreen();
void displayWiFiSetupScreen();
void drawSplitScreenLayout();
void drawWeatherData(const WeatherData &currentWeather, const HourlyForecast hourlyForecast[]);
void drawCalendarEvents(const CalendarEvents &events);
void drawWeatherIcon(int x, int y, int size, const String &iconCode);
void drawBatteryStatus(int x, int y);

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

#endif // DISPLAY_H
