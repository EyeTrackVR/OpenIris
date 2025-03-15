#include "DeviceMode.hpp"

DeviceModeManager* DeviceModeManager::instance = nullptr;

DeviceModeManager::DeviceModeManager() : currentMode(DeviceMode::USB_MODE) {}

DeviceModeManager::~DeviceModeManager() {
  preferences.end();
}

void DeviceModeManager::init() {
  preferences.begin(PREF_NAMESPACE, false);
  
  // Load the saved mode or use default (USB_MODE)
  int savedMode = preferences.getInt(MODE_KEY, static_cast<int>(DeviceMode::AUTO_MODE));
  currentMode = static_cast<DeviceMode>(savedMode);
  
  // If in AUTO_MODE, determine the appropriate mode based on saved credentials
  if (currentMode == DeviceMode::AUTO_MODE) {
    currentMode = determineMode();
  }
  
  log_i("[DeviceModeManager] Initialized with mode: %d", static_cast<int>(currentMode));
}

DeviceMode DeviceModeManager::getMode() {
  return currentMode;
}

void DeviceModeManager::setMode(DeviceMode mode) {
  currentMode = mode;
  preferences.putInt(MODE_KEY, static_cast<int>(mode));
  log_i("[DeviceModeManager] Mode set to: %d", static_cast<int>(mode));
}

bool DeviceModeManager::hasWiFiCredentials() {
  return preferences.getBool(HAS_WIFI_CREDS_KEY, false);
}

void DeviceModeManager::setHasWiFiCredentials(bool hasCredentials) {
  preferences.putBool(HAS_WIFI_CREDS_KEY, hasCredentials);
  log_i("[DeviceModeManager] WiFi credentials status set to: %d", hasCredentials);
}

DeviceMode DeviceModeManager::determineMode() {
  // If WiFi credentials are saved, use WiFi mode, otherwise use AP mode
  return hasWiFiCredentials() ? DeviceMode::WIFI_MODE : DeviceMode::AP_MODE;
}

DeviceModeManager* DeviceModeManager::getInstance() {
  if (instance == nullptr) {
    createInstance();
  }
  return instance;
}

void DeviceModeManager::createInstance() {
  if (instance == nullptr) {
    instance = new DeviceModeManager();
    instance->init();
  }
}