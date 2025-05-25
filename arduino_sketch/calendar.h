#ifndef CALENDAR_H
#define CALENDAR_H

#include <Arduino.h>
#include <vector>
#include "config.h"

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

// Function declarations
bool getCalendarEvents(CalendarEvents &events);
bool authenticateMicrosoft();
bool refreshMicrosoftToken();

// Authenticate with Microsoft OAuth
bool authenticateMicrosoft() {
  // This is a simplified implementation
  // In a real application, you would need to implement the OAuth 2.0 flow
  // including device code flow or authorization code flow
  
  Serial.println("Microsoft authentication not fully implemented");
  Serial.println("In a real application, you would need to implement the OAuth 2.0 flow");
  
  // For testing purposes, assume authentication succeeded
  return true;
}

// Refresh Microsoft access token using refresh token
bool refreshMicrosoftToken() {
  extern Config config;
  
  if (config.msftRefreshToken.isEmpty()) {
    Serial.println("No refresh token available");
    return false;
  }
  
  HTTPClient http;
  http.begin("https://login.microsoftonline.com/common/oauth2/v2.0/token");
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  
  String postData = "client_id=" + config.msftClientId;
  postData += "&refresh_token=" + config.msftRefreshToken;
  postData += "&grant_type=refresh_token";
  postData += "&client_secret=" + config.msftClientSecret;
  
  int httpCode = http.POST(postData);
  if (httpCode != HTTP_CODE_OK) {
    Serial.printf("Token refresh failed, error: %d\n", httpCode);
    http.end();
    return false;
  }
  
  String payload = http.getString();
  http.end();
  
  // Parse JSON response
  DynamicJsonDocument doc(4096);
  DeserializationError error = deserializeJson(doc, payload);
  
  if (error) {
    Serial.print("Token JSON parsing failed: ");
    Serial.println(error.c_str());
    return false;
  }
  
  // Extract and save new refresh token
  if (doc.containsKey("refresh_token")) {
    config.msftRefreshToken = doc["refresh_token"].as<String>();
    saveConfig();
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

#endif // CALENDAR_H
