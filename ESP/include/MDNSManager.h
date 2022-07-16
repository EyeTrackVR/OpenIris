#pragma once
#include <ESPmDNS.h>
#include "StateManager.h"
namespace MDNSHandler
{
  void setupMDNS(const char *trackerName, StateManager * stateManager);
}
