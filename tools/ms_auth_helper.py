#!/usr/bin/env python3
"""
Microsoft OAuth Helper for ESP32 E-Ink Weather and Calendar Display

This script helps with the initial setup of Microsoft OAuth authentication
for accessing Outlook calendar data. It implements the OAuth 2.0 authorization
code flow and generates a refresh token that can be used by the ESP32.

Usage:
  python ms_auth_helper.py

Requirements:
  - Python 3.6+
  - requests
  - flask

Install dependencies:
  pip install requests flask
"""

import os
import sys
import json
import webbrowser
import base64
import requests
from urllib.parse import urlencode
from flask import Flask, request, redirect

# Configuration
CLIENT_ID = ""  # Your Microsoft application client ID
CLIENT_SECRET = ""  # Your Microsoft application client secret
REDIRECT_URI = "http://localhost:8000/callback"
SCOPES = ["Calendars.Read", "offline_access"]

# Flask app for handling the OAuth callback
app = Flask(__name__)

@app.route("/")
def home():
    """Home page with authentication link"""
    auth_url = (
        "https://login.microsoftonline.com/common/oauth2/v2.0/authorize?"
        + urlencode({
            "client_id": CLIENT_ID,
            "response_type": "code",
            "redirect_uri": REDIRECT_URI,
            "scope": " ".join(SCOPES),
            "response_mode": "query"
        })
    )
    return f"""
    <h1>Microsoft OAuth Helper</h1>
    <p>Click the button below to authenticate with Microsoft:</p>
    <a href="{auth_url}"><button>Authenticate</button></a>
    """

@app.route("/callback")
def callback():
    """Handle the OAuth callback"""
    code = request.args.get("code")
    if not code:
        return "Error: No authorization code received"
    
    # Exchange authorization code for tokens
    token_url = "https://login.microsoftonline.com/common/oauth2/v2.0/token"
    token_data = {
        "client_id": CLIENT_ID,
        "scope": " ".join(SCOPES),
        "code": code,
        "redirect_uri": REDIRECT_URI,
        "grant_type": "authorization_code",
        "client_secret": CLIENT_SECRET
    }
    
    response = requests.post(token_url, data=token_data)
    if response.status_code != 200:
        return f"Error: Failed to get tokens. {response.text}"
    
    tokens = response.json()
    
    # Save tokens to file
    with open("ms_tokens.json", "w") as f:
        json.dump(tokens, f, indent=2)
    
    # Generate code for ESP32
    refresh_token = tokens.get("refresh_token", "")
    esp32_code = f"""
    // Add this to your secrets.h file:
    const char* MICROSOFT_REFRESH_TOKEN = "{refresh_token}";
    """
    
    with open("esp32_ms_token.h", "w") as f:
        f.write(esp32_code)
    
    return f"""
    <h1>Authentication Successful!</h1>
    <p>Tokens have been saved to <code>ms_tokens.json</code></p>
    <p>ESP32 code has been generated in <code>esp32_ms_token.h</code></p>
    <p>Copy the contents of this file to your <code>secrets.h</code> file.</p>
    <h2>ESP32 Code:</h2>
    <pre>{esp32_code}</pre>
    <p><strong>Note:</strong> Keep your refresh token secure!</p>
    """

def main():
    """Main function"""
    print("Microsoft OAuth Helper for ESP32 E-Ink Weather and Calendar Display")
    print("=================================================================")
    
    # Check if client ID and secret are set
    global CLIENT_ID, CLIENT_SECRET
    if not CLIENT_ID or not CLIENT_SECRET:
        print("Please enter your Microsoft application credentials:")
        CLIENT_ID = input("Client ID: ").strip()
        CLIENT_SECRET = input("Client Secret: ").strip()
        
        if not CLIENT_ID or not CLIENT_SECRET:
            print("Error: Client ID and Client Secret are required")
            sys.exit(1)
    
    # Start the web server
    print(f"\nStarting authentication server at {REDIRECT_URI}")
    print("Opening browser for authentication...")
    webbrowser.open("http://localhost:8000/")
    
    # Run the Flask app
    app.run(host="localhost", port=8000)

if __name__ == "__main__":
    main()
