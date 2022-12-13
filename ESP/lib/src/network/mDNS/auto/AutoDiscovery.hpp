#ifndef AutoDiscovery_hpp
#define AutoDiscovery_hpp

#include <ESPmDNS.h>
#include <WiFi.h>
#include "data/StateManager/StateManager.hpp"
#include "data/utilities/Observer.hpp"
#include "data/utilities/helpers.hpp"
#include "data/config/project_config.hpp"

class AutoDiscovery : public IObserver
{
private:
    StateManager<MDNSState_e> *stateManager;
    ProjectConfig *configManager;
    ProjectConfig::MDNSConfig_t *mdnsConfig;
    void (*callback)(const char *);
    int _queryTimeout;
    unsigned long _currentTime;
    unsigned long _previousTime;

    void queryService(const std::string &service, const std::string &protocol);
    void stop();

public:
    AutoDiscovery(StateManager<MDNSState_e> *stateManager,
                  ProjectConfig *configManager);
    ~AutoDiscovery();
    void start();
    void update(ObserverEvent::Event event);
};

#endif // AutoDiscovery_hpp