#pragma once
#ifndef QUERYMDNSSERVICE_HPP
#define QUERYMDNSSERVICE_HPP
#include <ESPmDNS.h>
#include "data/StateManager/StateManager.hpp"
#include "data/utilities/Observer.hpp"
#include "data/utilities/helpers.hpp"
#include "data/config/project_config.hpp"

class QueryMDNSService : public IObserver
{
private:
    StateManager<MDNSState_e> *stateManager;
    ProjectConfig *configManager;

public:
    QueryMDNSService(StateManager<MDNSState_e> *stateManager,
                     ProjectConfig *configManager);
    void queryMDNS();
    void update(ObserverEvent::Event event);
};

#endif // QUERYMDNSSERVICE_HPP