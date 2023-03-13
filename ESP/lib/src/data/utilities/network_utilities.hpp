#pragma once
#ifndef UTILITIES_hpp
#define UTILITIES_hpp
#include <Arduino.h>
#include <WiFi.h>
#include <data/StateManager/StateManager.hpp>
#include <unordered_map>
namespace Network_Utilities {
  bool LoopWifiScan();
  void SetupWifiScan();
  void my_delay(volatile long delay_time);
  void checkWifiState();
  std::string generateDeviceID();
  int getStrength(int points);
}  // namespace Network_Utilities
#endif  // !UTILITIES_hpp
