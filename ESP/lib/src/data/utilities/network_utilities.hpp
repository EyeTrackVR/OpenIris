#pragma once
#ifndef UTILITIES_hpp
#define UTILITIES_hpp
#include <Arduino.h>
#include <WiFi.h>
#include <unordered_map>
#include <data/StateManager/StateManager.hpp>
#include "mbedtls/md.h"

namespace Network_Utilities
{
    bool loopWifiScan();
    void setupWifiScan();
    void my_delay(volatile long delay_time);
    int getStrength(int points);
    void checkWiFiState();
}
#endif // !UTILITIES_hpp