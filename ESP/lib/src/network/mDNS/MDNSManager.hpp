#pragma once
#include <ESPmDNS.h>
#include "data/StateManager/StateManager.hpp"
#include "data/Observer/Observer.h"
#include "data/config/project_config.hpp"

class MDNSHandler : public IObserver
{
private:
  StateManager<ProgramStates::DeviceStates::MDNSState_e> *stateManager;
  ProjectConfig *configManager;

public:
  MDNSHandler(StateManager<ProgramStates::DeviceStates::MDNSState_e> *stateManager, Configuration *trackerConfig) : stateManager(stateManager), trackerConfig(trackerConfig) {}
  void startMDNS();
  void update(ObserverEvent::Event event);
};