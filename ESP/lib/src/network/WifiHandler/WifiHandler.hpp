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
  WiFiHandler(ProjectConfig *configManager, StateManager<ProgramStates::DeviceStates::WiFiState_e> *stateManager);
  virtual ~WiFiHandler();
  void setupWifi();
  void setUpADHOC();
  void setWiFiConf(const char *value, uint8_t *location);
  std::unique_ptr<wifi_config_t> conf;

private:
  ProjectConfig *configManager;
  StateManager<ProgramStates::DeviceStates::WiFiState_e> *stateManager;
};
#endif // WIFIHANDLER_HPP
