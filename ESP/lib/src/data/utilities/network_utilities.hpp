#pragma once
#ifndef UTILITIES_hpp
#define UTILITIES_hpp
#include <Arduino.h>
#include <WiFi.h>
#include <data/StateManager/StateManager.hpp>
#include <unordered_map>
namespace Network_Utilities {
  bool loopWifiScan();
  void setupWifiScan();
  void my_delay(volatile long delay_time);
  void checkWiFiState();
  std::string generateDeviceID();
  int getStrength(int points);
}  // namespace Network_Utilities
#endif  // !UTILITIES_hpp
