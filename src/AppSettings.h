#pragma once

// Settings structure shared across the application
struct AppSettings
{
    bool showTimestamps = true;
    bool use24HourFormat = true;  // true = 24-hour, false = 12-hour
    bool autoReconnect = true;     // Automatically reconnect on disconnect
    int maxReconnectAttempts = 5;  // Max reconnect attempts (0 = unlimited)
};
