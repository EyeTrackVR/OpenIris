#pragma once
#ifndef QUERYMDNSSERVICE_HPP
#define QUERYMDNSSERVICE_HPP
#include <Arduino.h>
#include <ESPmDNS.h>
#include "data/StateManager/StateManager.hpp"
#include "data/utilities/Observer.hpp"
#include "data/utilities/helpers.hpp"
#include "data/config/project_config.hpp"

class QueryMDNSService
{
private:
    StateManager<MDNSState_e> *stateManager;
    ProjectConfig *configManager;

public:
    QueryMDNSService(StateManager<MDNSState_e> *stateManager,
                     ProjectConfig *configManager);
    virtual ~QueryMDNSService();
    void queryMDNS();
};

#endif // QUERYMDNSSERVICE_HPP