#pragma once
#ifndef WIFIHANDLER_HPP
#define WIFIHANDLER_HPP
#include <memory>
#include <WiFi.h>
#include "data/StateManager/StateManager.hpp"
#include "data/config/project_config.hpp"

extern "C"
{
#include <esp_err.h>
#include <esp_wifi.h>
#include <esp_event.h>
}

class WiFiHandler
{
public:
  WiFiHandler();
  virtual ~WiFiHandler();
  void setupWifi();
  void setUpADHOC();
  void setWiFiConf(const char *value, uint8_t *location);
  std::unique_ptr<wifi_config_t> conf;
  std::shared_ptr<StateManager<ProgramStates::DeviceStates::WiFiState_e>> wifiStateManager;
  std::shared_ptr<ProjectConfig> configManager;
};
extern WiFiHandler wifiHandler;
#endif // WIFIHANDLER_HPP
