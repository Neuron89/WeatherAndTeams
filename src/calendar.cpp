#include "calendar.h"
#include "config.h"
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>

// Microsoft Graph API endpoint
const char* GRAPH_API_ENDPOINT = "https://graph.microsoft.com/v1.0/me/calendarview";
// Microsoft OAuth endpoints
const char* MS_AUTH_ENDPOINT = "https://login.microsoftonline.com/common/oauth2/v2.0/token";

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
  // Access the global config variable
  extern Config config;
  
  if (config.msftRefreshToken.isEmpty()) {
    Serial.println("No refresh token available");
    return false;
  }
  
  HTTPClient http;
  http.begin(MS_AUTH_ENDPOINT);
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
  // Check if we need to authenticate or refresh token
  if (config.msftRefreshToken.isEmpty()) {
    if (!authenticateMicrosoft()) {
      Serial.println("Microsoft authentication failed");
      return false;
    }
  } else {
    if (!refreshMicrosoftToken()) {
      Serial.println("Microsoft token refresh failed");
      return false;
    }
  }
  
  // Calculate time range for calendar events (today and next 7 days)
  time_t now = time(nullptr);
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  
  // Set to start of day
  timeinfo.tm_hour = 0;
  timeinfo.tm_min = 0;
  timeinfo.tm_sec = 0;
  time_t startTime = mktime(&timeinfo);
  
  // Set to 7 days later
  timeinfo.tm_mday += 7;
  time_t endTime = mktime(&timeinfo);
  
  // Format times for API request
  char startTimeStr[30], endTimeStr[30];
  strftime(startTimeStr, sizeof(startTimeStr), "%Y-%m-%dT%H:%M:%SZ", gmtime(&startTime));
  strftime(endTimeStr, sizeof(endTimeStr), "%Y-%m-%dT%H:%M:%SZ", gmtime(&endTime));
  
  // Prepare API request
  String url = String(GRAPH_API_ENDPOINT);
  url += "?startDateTime=" + String(startTimeStr);
  url += "&endDateTime=" + String(endTimeStr);
  url += "&$select=subject,start,end,location,isAllDay";
  url += "&$orderby=start/dateTime";
  url += "&$top=50";
  
  // This is a simplified implementation
  // In a real application, you would need to include the access token in the request
  
  Serial.println("Calendar API request not fully implemented");
  Serial.println("In a real application, you would make an authenticated request to Microsoft Graph API");
  
  // For testing purposes, create some dummy events
  events.events.clear();
  
  // Add dummy events
  CalendarEvent event1;
  event1.title = "Team Meeting";
  event1.location = "Conference Room A";
  event1.startTime = now + 3600; // 1 hour from now
  event1.endTime = now + 7200;   // 2 hours from now
  event1.isAllDay = false;
  events.events.push_back(event1);
  
  CalendarEvent event2;
  event2.title = "Project Deadline";
  event2.location = "";
  event2.startTime = now + 86400; // Tomorrow
  event2.endTime = now + 86400;
  event2.isAllDay = true;
  events.events.push_back(event2);
  
  events.lastUpdated = now;
  
  return true;
}
