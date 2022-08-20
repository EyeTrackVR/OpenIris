#pragma once
#ifndef UTILITIES_hpp
#define UTILITIES_hpp
#include <Arduino.h>
#include "network/wifihandler/WifiHandler.hpp"
#include <unordered_map>
namespace Network_Utilities
{
    bool LoopWifiScan();
    void SetupWifiScan();
    void my_delay(volatile long delay_time);
    int CheckWifiState();
    int getStrength(int points);
    String generateDeviceID();
}
#endif // !UTILITIES_hpp