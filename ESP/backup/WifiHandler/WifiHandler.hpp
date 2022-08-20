#pragma once
#ifndef WIFIHANDLER_HPP
#define WIFIHANDLER_HPP
#include <memory>
#include <WiFi.h>
#include "data/StateManager/StateManager.hpp"
#include "data/config/project_config.hpp"

class WiFiHandler
{
public:
  WiFiHandler(ProjectConfig *configManager, StateManager<WiFiState_e> *stateManager);
  virtual ~WiFiHandler();
  void setupWifi();
  ProjectConfig *configManager;
  StateManager<WiFiState_e> *stateManager;
private:
  void setUpADHOC();
  void adhoc(const char *ssid, const char *password, uint8_t channel);
  void iniSTA();
};
#endif // WIFIHANDLER_HPP
