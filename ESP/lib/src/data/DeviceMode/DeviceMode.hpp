#pragma once
#ifndef DEVICE_MODE_HPP
#define DEVICE_MODE_HPP

#include <Arduino.h>
#include <Preferences.h>
#include <string>

// Enum to represent the device operating mode
enum class DeviceMode {
  USB_MODE,    // Device operates in USB mode only
  WIFI_MODE,   // Device operates in WiFi mode only
  AP_MODE,     // Device operates in AP mode with serial commands enabled
  AUTO_MODE    // Device automatically selects mode based on saved credentials
};

class DeviceModeManager {
private:
  static DeviceModeManager* instance;
  Preferences preferences;
  DeviceMode currentMode;
  const char* PREF_NAMESPACE = "device_mode";
  const char* MODE_KEY = "mode";
  const char* HAS_WIFI_CREDS_KEY = "has_wifi_creds";

public:
  DeviceModeManager();
  ~DeviceModeManager();
  
  static DeviceModeManager* getInstance();
  
  static void createInstance();
  
  void init();
  
  DeviceMode getMode();
  
  void setMode(DeviceMode mode);
  
  bool hasWiFiCredentials();
  
  void setHasWiFiCredentials(bool hasCredentials);
  
  DeviceMode determineMode();
};

#endif // DEVICE_MODE_HPP