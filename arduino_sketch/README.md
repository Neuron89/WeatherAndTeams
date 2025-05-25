# Arduino Sketch for E-Ink Weather and Calendar Display

This folder contains a complete Arduino sketch for the ESP32 E-Ink Weather and Calendar Display project. You can simply copy this entire folder and open it directly in the Arduino IDE.

## Files in this Sketch

- `Eink_Weather_Calendar.ino` - Main Arduino sketch file
- `secrets.h` - Configuration file for API keys (edit this with your credentials)
- `config.h` - Configuration management
- `weather.h` - Weather data structures and functions
- `calendar.h` - Calendar data structures and functions
- `display.h` - Display functions

## Setup Instructions

1. Copy this entire folder to your Arduino sketches directory
2. Open `Eink_Weather_Calendar.ino` in Arduino IDE
3. Install the required libraries through the Arduino Library Manager:
   - GxEPD2 by Jean-Marc Zingg
   - ArduinoJson by Benoit Blanchon
   - WiFiManager by tzapu
   - Time by Paul Stoffregen

4. Edit `secrets.h` with your API keys
5. Select the ESP32 board from the Tools menu:
   - Tools → Board → ESP32 Arduino → ESP32 Dev Module
   - Tools → Partition Scheme → "Huge APP" (to have enough space for all libraries)
   - Tools → Port → (select the port your ESP32 is connected to)

6. Compile and upload to your ESP32

## Hardware Connections

Connect the Waveshare 7.5-inch E-Paper HAT (H) to the ESP32 as follows:

| E-Paper Display | ESP32 |
|-----------------|-------|
| BUSY            | GPIO25|
| RST             | GPIO26|
| DC              | GPIO27|
| CS              | GPIO15|
| CLK             | GPIO13|
| DIN             | GPIO14|
| GND             | GND   |
| VCC             | 3.3V  |

## Troubleshooting

If you encounter compilation errors:
- Make sure all required libraries are installed
- Try changing the Partition Scheme to "Huge APP" in the Tools menu
- Check that your ESP32 board support is properly installed
