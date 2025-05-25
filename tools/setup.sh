#!/bin/bash
# Setup script for E-Ink Weather and Calendar Display

echo "E-Ink Weather and Calendar Display Setup"
echo "========================================"
echo

# Check if PlatformIO is installed
if ! command -v pio &> /dev/null; then
    echo "PlatformIO is not installed. Please install it first:"
    echo "https://platformio.org/install/cli"
    exit 1
fi

# Create secrets.h from template
if [ ! -f "../include/secrets.h" ]; then
    echo "Creating secrets.h from template..."
    cp ../include/secrets_template.h ../include/secrets.h
    echo "Created secrets.h"
    echo "Please edit include/secrets.h to add your API keys"
else
    echo "secrets.h already exists"
fi

# Ask for OpenWeatherMap API key
read -p "Enter your OpenWeatherMap API key (leave blank to skip): " owm_key
if [ ! -z "$owm_key" ]; then
    # Update the API key in secrets.h
    sed -i "s/your_api_key_here/$owm_key/" ../include/secrets.h
    echo "Updated OpenWeatherMap API key"
fi

# Ask if user wants to set up Microsoft authentication
read -p "Do you want to set up Microsoft authentication now? (y/n): " ms_auth
if [ "$ms_auth" = "y" ]; then
    echo "Starting Microsoft authentication helper..."
    python3 ms_auth_helper.py
else
    echo "You can set up Microsoft authentication later by running:"
    echo "  cd tools && python3 ms_auth_helper.py"
fi

# Generate weather icons
read -p "Do you want to generate weather icons now? (y/n): " gen_icons
if [ "$gen_icons" = "y" ]; then
    echo "Generating weather icons..."
    python3 generate_weather_icons.py
else
    echo "You can generate weather icons later by running:"
    echo "  cd tools && python3 generate_weather_icons.py"
fi

echo
echo "Setup completed!"
echo "You can now build and upload the firmware using PlatformIO:"
echo "  cd .. && pio run -t upload"
echo
echo "For more information, see the documentation in the docs/ directory."
