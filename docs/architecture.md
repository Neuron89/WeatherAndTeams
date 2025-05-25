# E-Ink Weather and Calendar Display Architecture

## System Architecture

```
+------------------+     +------------------+     +------------------+
|                  |     |                  |     |                  |
|  Weather API     |     |  IP Geolocation  |     |  Microsoft Graph |
|  (OpenWeatherMap)|     |  API             |     |  API (Outlook)   |
|                  |     |                  |     |                  |
+--------+---------+     +--------+---------+     +--------+---------+
         |                        |                        |
         |                        |                        |
         v                        v                        v
+--------------------------------------------------+----------------+
|                                                  |                |
|                      ESP32                       |   E-Ink        |
|                                                  |   Display      |
|  +----------------+  +----------------+          |   (7.5")       |
|  |                |  |                |          |                |
|  |  WiFi Manager  |  |  Data Manager  |          |                |
|  |  (Setup Portal)|  |  (API Clients) |          |                |
|  |                |  |                |          |                |
|  +----------------+  +----------------+          |                |
|                                                  |                |
|  +----------------+  +----------------+          |                |
|  |                |  |                |          |                |
|  |  Weather       |  |  Calendar      |          |                |
|  |  Renderer      |  |  Renderer      |          |                |
|  |                |  |                |          |                |
|  +----------------+  +----------------+          |                |
|                                                  |                |
+--------------------------------------------------+----------------+
```

## Data Flow

1. **Initialization**:
   - ESP32 boots up
   - If not configured, starts WiFi Manager in AP mode
   - User connects to AP and configures WiFi credentials
   - ESP32 connects to configured WiFi network

2. **Data Acquisition**:
   - ESP32 determines location via IP Geolocation API
   - ESP32 fetches weather data from OpenWeatherMap API
   - ESP32 authenticates with Microsoft and fetches calendar data

3. **Display Rendering**:
   - Weather data is rendered on the left side of the display
   - Calendar data is rendered on the right side of the display
   - E-Ink display is updated

4. **Power Management**:
   - ESP32 enters deep sleep mode to conserve power
   - ESP32 wakes up periodically to refresh data

## Component Descriptions

### WiFi Manager
Provides a captive portal for initial WiFi setup, allowing the user to connect the device to their home network without hardcoding credentials.

### Data Manager
Handles API requests to external services, including authentication, data fetching, and parsing.

### Weather Renderer
Processes weather data and renders it on the left side of the E-Ink display, including current conditions and hourly forecast.

### Calendar Renderer
Processes calendar data and renders it on the right side of the E-Ink display, showing upcoming events in chronological order.

### E-Ink Display Driver
Manages communication with the Waveshare 7.5-inch E-Ink display, handling initialization, drawing, and updates.
