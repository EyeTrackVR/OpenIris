#pragma once
#include <WiFi.h>
#include "pinout.h"

namespace OpenIris {
    namespace WiFiHandler {
        void setupWifi(const char *ssid, const char *password);
    }
}