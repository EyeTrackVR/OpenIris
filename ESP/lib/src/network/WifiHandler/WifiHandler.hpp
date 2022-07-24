#pragma once
#include <WiFi.h>
#include "data/StateManager/StateManager.hpp"
#include "data/config/project_config.hpp"

namespace WiFiHandler
{
  void setupWifi(StateManager<ProgramStates::DeviceStates::WiFiState_e> *stateManager, ProjectConfig *configManager);
}
