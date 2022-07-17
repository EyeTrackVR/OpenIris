#pragma once
#include <WiFi.h>
#include "pinout.h"
#include "../../data/StateManager/StateManager.hpp"

namespace WiFiHandler
{
  void setupWifi(const char *ssid, const char *password, StateManager<ProgramStates::DeviceStates::WiFiState_e> *stateManager);
}
