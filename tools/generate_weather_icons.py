#!/usr/bin/env python3
"""
Weather Icon Generator for E-Ink Display

This script downloads weather icons from OpenWeatherMap and converts them
to 1-bit BMP format suitable for E-Ink displays.

Usage:
  python generate_weather_icons.py

Requirements:
  - Python 3.6+
  - requests
  - Pillow (PIL)

Install dependencies:
  pip install requests pillow
"""

import os
import sys
import requests
from PIL import Image, ImageOps

# Configuration
ICON_CODES = [
    "01d", "01n",  # Clear sky
    "02d", "02n",  # Few clouds
    "03d", "03n",  # Scattered clouds
    "04d", "04n",  # Broken clouds
    "09d", "09n",  # Shower rain
    "10d", "10n",  # Rain
    "11d", "11n",  # Thunderstorm
    "13d", "13n",  # Snow
    "50d", "50n",  # Mist
]

ICON_SIZES = [
    (100, 100),  # Large icons for current weather
    (30, 30),    # Small icons for hourly forecast
]

ICON_BASE_URL = "https://openweathermap.org/img/wn/{code}@2x.png"
OUTPUT_DIR = "../assets"

def download_icon(icon_code):
    """Download weather icon from OpenWeatherMap"""
    url = ICON_BASE_URL.format(code=icon_code)
    response = requests.get(url, stream=True)
    if response.status_code != 200:
        print(f"Error downloading icon {icon_code}: {response.status_code}")
        return None
    
    # Create a temporary file to save the downloaded image
    img = Image.open(response.raw)
    return img

def convert_to_bmp(img, size):
    """Convert image to 1-bit BMP format and resize"""
    # Resize the image
    img = img.resize(size, Image.LANCZOS)
    
    # Convert to grayscale
    img = img.convert("L")
    
    # Convert to 1-bit using dithering
    img = img.convert("1", dither=Image.FLOYDSTEINBERG)
    
    return img

def main():
    """Main function"""
    print("Weather Icon Generator for E-Ink Display")
    print("=======================================")
    
    # Create output directory if it doesn't exist
    os.makedirs(OUTPUT_DIR, exist_ok=True)
    
    # Process each icon code
    for icon_code in ICON_CODES:
        print(f"Processing icon: {icon_code}")
        
        # Download the icon
        img = download_icon(icon_code)
        if img is None:
            continue
        
        # Convert and save in different sizes
        for size in ICON_SIZES:
            size_suffix = f"{size[0]}x{size[1]}"
            output_filename = os.path.join(OUTPUT_DIR, f"{icon_code}_{size_suffix}.bmp")
            
            # Convert to BMP
            bmp_img = convert_to_bmp(img, size)
            
            # Save the image
            bmp_img.save(output_filename)
            print(f"  Saved: {output_filename}")
    
    print("\nIcon generation complete!")

if __name__ == "__main__":
    main()
