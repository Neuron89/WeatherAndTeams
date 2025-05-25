# Setup Guide

This guide will help you set up your ESP32 E-Ink Weather and Calendar Display.

## Hardware Setup

1. Connect the Waveshare 7.5-inch E-Paper HAT (H) to the ESP32 E-Paper Driver Board:

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

2. Connect the ESP32 to your computer using a USB cable.

## Software Setup

### Prerequisites

- [PlatformIO](https://platformio.org/) installed (or PlatformIO extension for VSCode)
- [Git](https://git-scm.com/) installed
- OpenWeatherMap API key (get one for free at [OpenWeatherMap](https://home.openweathermap.org/api_keys))
- Microsoft Azure account for Outlook integration

### Step 1: Clone the Repository

```bash
git clone https://github.com/yourusername/eink-weather-calendar.git
cd eink-weather-calendar
```

### Step 2: Configure API Keys

1. Copy the template secrets file:

```bash
cp include/secrets_template.h include/secrets.h
```

2. Edit `include/secrets.h` and add your API keys:

```cpp
// OpenWeatherMap API Key
const char* OPENWEATHERMAP_API_KEY = "your_api_key_here";

// Microsoft Graph API credentials
const char* MICROSOFT_CLIENT_ID = "your_client_id_here";
const char* MICROSOFT_CLIENT_SECRET = "your_client_secret_here";
```

### Step 3: Build and Upload

1. Open the project in PlatformIO:

```bash
platformio open
```

2. Build the project:

```bash
platformio run
```

3. Upload to your ESP32:

```bash
platformio run --target upload
```

4. Monitor the serial output (optional):

```bash
platformio device monitor
```

## First-time Setup

1. Power on the device
2. Connect to the WiFi network named "EinkWeather_XXXXXX"
3. Open a web browser and navigate to 192.168.4.1
4. Follow the on-screen instructions to connect to your home WiFi network
5. The device will restart and connect to your WiFi network
6. Follow the Microsoft authentication process if prompted

## Troubleshooting

### Device not creating WiFi network

- Check power supply
- Press the reset button
- Ensure the firmware was uploaded correctly

### Cannot connect to WiFi

- Ensure your WiFi network is 2.4GHz (ESP32 doesn't support 5GHz)
- Check that your password is correct
- Try moving closer to your router

### Display not updating

- Check connections between ESP32 and E-Ink display
- Verify that the display is properly seated in the connector
- Press reset button to restart the device

### Weather data not showing

- Verify your OpenWeatherMap API key is correct
- Check that the device is connected to the internet
- Ensure the device can reach api.openweathermap.org

### Calendar data not showing

- Verify your Microsoft credentials
- Check that you've completed the authentication process
- Ensure the device can reach graph.microsoft.com
