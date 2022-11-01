#pragma once
#include <ESPmDNS.h>
#include "../../data/StateManager/StateManager.hpp"
namespace MDNSHandler
{
    void setupMDNS(const char *trackerName, StateManager<ProgramStates::DeviceStates::MDNSState_e> *stateManager);
}