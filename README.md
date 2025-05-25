# E-Ink Weather and Calendar Display

A project for ESP32 and Waveshare 7.5-inch E-Ink display that shows weather information and Microsoft Outlook calendar events on a split screen.

## Hardware Requirements

- ESP32 Development Board (ESP32 E-Paper Driver Board)
- Waveshare 7.5-inch E-Paper HAT (H) Display
- USB cable for power and programming

## Features

- **Split-screen display:**
  - Left side: Weather information
  - Right side: Microsoft Outlook calendar

- **Weather Features:**
  - Location detection via IP geolocation
  - Current weather with large icon, temperature, humidity
  - Hourly forecast with smaller icons

- **Calendar Features:**
  - Display of upcoming events from Microsoft Outlook
  - Event details including time, title, and location

- **Connectivity:**
  - WiFi setup via captive portal
  - Microsoft authentication for Outlook calendar access

## Setup Instructions

1. Clone this repository
2. Open the project in PlatformIO
3. Configure your OpenWeatherMap API key in `weather.cpp`
4. Configure your Microsoft application credentials in `calendar.cpp`
5. Build and upload the firmware to your ESP32

## First-time Setup

1. Power on the device
2. Connect to the WiFi network named "EinkWeather_XXXXXX"
3. Open a web browser and navigate to 192.168.4.1
4. Follow the on-screen instructions to connect to your home WiFi network
5. The device will restart and connect to your WiFi network
6. Follow the Microsoft authentication process if prompted

## Hardware Connections

The Waveshare 7.5-inch E-Paper HAT (H) connects to the ESP32 E-Paper Driver Board as follows:

| E-Paper Display | ESP32 E-Paper Driver Board |
|-----------------|----------------------------|
| BUSY            | GPIO25                     |
| RST             | GPIO26                     |
| DC              | GPIO27                     |
| CS              | GPIO15                     |
| CLK             | GPIO13                     |
| DIN             | GPIO14                     |
| GND             | GND                        |
| VCC             | 3.3V                       |

## Power Management

The device uses deep sleep mode to conserve power. It wakes up every 30 minutes to update the weather and calendar data, then goes back to sleep.

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Acknowledgments

- [GxEPD2 Library](https://github.com/ZinggJM/GxEPD2) for E-Paper display support
- [WiFiManager](https://github.com/tzapu/WiFiManager) for the captive portal
- [ArduinoJson](https://arduinojson.org/) for JSON parsing
- [OpenWeatherMap](https://openweathermap.org/) for weather data
- [Microsoft Graph API](https://developer.microsoft.com/en-us/graph) for calendar data
