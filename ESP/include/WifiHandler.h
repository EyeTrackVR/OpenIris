#pragma once
#include <WiFi.h>
#include "pinout.h"
#include "StateManager.h"

namespace WiFiHandler
{
  void setupWifi(const char *ssid, const char *password, StateManager *stateManager);
}
