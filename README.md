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

### Using PlatformIO

1. Clone this repository
2. Open the project in PlatformIO
3. Configure your OpenWeatherMap API key in `weather.cpp`
4. Configure your Microsoft application credentials in `calendar.cpp`
5. Build and upload the firmware to your ESP32

### Using Arduino IDE

1. Download the project files
2. Open `src/main.cpp` in Arduino IDE (rename it to `main.ino` if needed)
3. Install the required libraries through the Arduino Library Manager:
   - **GxEPD2** by Jean-Marc Zingg (for E-Ink display)
   - **ArduinoJson** by Benoit Blanchon
   - **WiFiManager** by tzapu
   - **Time** by Paul Stoffregen
   - **ESP32 Arduino** framework (should be installed with ESP32 board support)

4. Configure your board:
   - Tools → Board → ESP32 Arduino → ESP32 Dev Module
   - Tools → Partition Scheme → "Huge APP" (to have enough space for all libraries)
   - Tools → Port → (select the port your ESP32 is connected to)

5. Create a `secrets.h` file in the same directory as your sketch with your API keys:
   ```cpp
   #ifndef SECRETS_H
   #define SECRETS_H
   
   // OpenWeatherMap API Key
   const char* OPENWEATHERMAP_API_KEY = "your_api_key_here";
   
   // Microsoft Graph API credentials
   const char* MICROSOFT_CLIENT_ID = "your_client_id_here";
   const char* MICROSOFT_CLIENT_SECRET = "your_client_secret_here";
   
   #endif // SECRETS_H
   ```

6. Make sure all the project files are in the same sketch folder:
   - Copy all `.cpp` and `.h` files from `src/` and `include/` to your sketch folder
   - Create an `assets` folder in your sketch folder for weather icons

7. Compile and upload to your ESP32

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

## Troubleshooting Arduino IDE Issues

### Compilation Errors

- **Missing Libraries**: Make sure all required libraries are installed through the Library Manager
- **ESP32 Support**: Ensure you have ESP32 board support installed via Boards Manager
- **Memory Issues**: If you get memory-related errors, try changing the Partition Scheme to "Huge APP"
- **Multiple Definitions**: If you get "multiple definition" errors, make sure you're not including .cpp files, only .h files

### Upload Issues

- **Port Not Found**: Make sure your ESP32 is connected and the correct port is selected
- **Upload Failed**: Try holding the BOOT button on your ESP32 while uploading
- **Connection Issues**: Try a different USB cable or port
- **Driver Issues**: Make sure you have the correct USB-to-Serial drivers installed

### Display Issues

- **Display Not Updating**: Check the wiring connections between ESP32 and E-Ink display
- **Partial Updates**: E-Ink displays sometimes need a full refresh; try power cycling the device
- **Incorrect Orientation**: Adjust the rotation parameter in `display.setRotation()`

### WiFi Issues

- **Cannot Connect**: Make sure you're using a 2.4GHz network (ESP32 doesn't support 5GHz)
- **Captive Portal Not Working**: Try accessing 192.168.4.1 directly in your browser
- **Connection Drops**: Check your power supply; unstable power can cause WiFi issues
