#include "AutoDiscovery.hpp"

AutoDiscovery::AutoDiscovery(StateManager<MDNSState_e> *stateManager,
                             ProjectConfig *configManager) : stateManager(stateManager),
                                                             configManager(configManager),
                                                             mdnsConfig(NULL),
                                                             callback(NULL),
                                                             _queryTimeout(5000),
                                                             _currentTime(millis()),
                                                             _previousTime(0)
{
    if (mdns_init() != ESP_OK)
    {
        stateManager->setState(MDNSState_e::MDNSState_Error);
        return;
    }
    mdnsConfig = configManager->getMDNSConfig();
}

AutoDiscovery::~AutoDiscovery()
{
    stop();
}

void AutoDiscovery::start()
{
    stateManager->setState(MDNSState_e::MDNSState_QueryStarted);
    queryService(("_" + mdnsConfig->hostname).c_str(), "_tcp");
    stop();
}

/**
 * @brief Query for the number of services on the network
 *
 * @param service string - the number of services found
 * @param protocol string - the protocol of the service
 * @return int - the number of services found
 */
void AutoDiscovery::queryService(const std::string &service, const std::string &protocol)
{
    if (stateManager->getCurrentState() != MDNSState_e::MDNSState_QueryComplete)
    {
        int numServices = 0;
        if (_currentTime - _previousTime >= _queryTimeout)
        {
            _previousTime = _currentTime;
            int n = MDNS.queryService(service.c_str(), protocol.c_str());
            if (n != numServices)
            {
                numServices = n;
            }
        }
        if (numServices == 0)
        {
            log_w("no services found");
            // set the hostname for mDNS - this is what we will need to connect to
            if (mdnsConfig->hostname != "openiristracker")
            {
                log_w("No hostname set, using default");
                configManager->setMDNSConfig(MDNS_HOSTNAME, mdnsConfig->hostname.c_str(), true);
                return;
            }
            log_i("Setting hostname to %s", mdnsConfig->hostname.c_str());
            configManager->setMDNSConfig(mdnsConfig->hostname.c_str(), mdnsConfig->service.c_str(), true);
            return;
        }

        log_i("%d services found", numServices);
        for (int i = 0; i < numServices; ++i)
        {
            // print details for each service found
            log_i("service #%d: %s - %s:%d", i + 1, MDNS.hostname(i).c_str(), MDNS.IP(i).toString().c_str(), MDNS.port(i));
            return;
        }
        std::string iterator = std::to_string(numServices);
        std::string hostname = mdnsConfig->hostname.assign(iterator);

        if (mdnsConfig->hostname != "openiristracker")
        {
            log_w("No hostname set, using default");
            configManager->setMDNSConfig(MDNS_HOSTNAME, mdnsConfig->hostname.c_str(), true);
            return;
        }
        log_i("Setting hostname to %s", hostname.c_str());
        configManager->setMDNSConfig(hostname.c_str(), mdnsConfig->service.c_str(), true);
        return;
    }
}

void AutoDiscovery::stop()
{
    stateManager->setState(MDNSState_e::MDNSState_QueryComplete);
}

void AutoDiscovery::setCallback(void (*callback)(const char *))
{
    this->callback = callback;
}

void AutoDiscovery::update(ObserverEvent::Event event)
{
    switch (event)
    {
    case ObserverEvent::Event::mdnsConfigUpdated:
        MDNS.end();
        start();
        break;
    default:
        break;
    }
}

// Path: lib\src\network\mDNS\auto\AutoDiscovery.hpp