#pragma once
#include <ESPmDNS.h>
#include "data/StateManager/StateManager.hpp"
#include "data/utilities/Observer.hpp"
#include "data/config/project_config.hpp"

class MDNSHandler : public IObserver
{
private:
  StateManager<ProgramStates::DeviceStates::MDNSState_e> *stateManager;
  ProjectConfig *configManager;

public:
  MDNSHandler(StateManager<ProgramStates::DeviceStates::MDNSState_e> *stateManager, ProjectConfig *configManager) : stateManager(stateManager), configManager(configManager) {}
  void startMDNS();
  void update(ObserverEvent::Event event);
};