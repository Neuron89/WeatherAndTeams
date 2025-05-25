#ifndef SECRETS_H
#define SECRETS_H

// OpenWeatherMap API Key
// Get yours at: https://home.openweathermap.org/api_keys
const char* OPENWEATHERMAP_API_KEY = "your_api_key_here";

// Microsoft Graph API credentials
// Register your application at: https://portal.azure.com/#blade/Microsoft_AAD_RegisteredApps/ApplicationsListBlade
const char* MICROSOFT_CLIENT_ID = "your_client_id_here";
const char* MICROSOFT_CLIENT_SECRET = "your_client_secret_here";

// Redirect URI for OAuth flow
const char* MICROSOFT_REDIRECT_URI = "http://localhost";

#endif // SECRETS_H
