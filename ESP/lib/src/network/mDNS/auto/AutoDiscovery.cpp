#include "AutoDiscovery.hpp"

AutoDiscovery::AutoDiscovery(StateManager<MDNSState_e> *stateManager,
                             ProjectConfig *configManager) : stateManager(stateManager),
                                                             configManager(configManager),
                                                             mdnsConfig(NULL),
                                                             callback(NULL)
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
    // run the queryService function every 5 seconds
    // this will query for the service and print the results to the serial monitor
    // the queryService function will also set the hostname for mDNS
    int services = queryService(mdnsConfig->hostname, "tcp");
}

/**
 * @brief Query for the number of services on the network
 *
 * @param service string - the number of services found
 * @param protocol string - the protocol of the service
 * @return int - the number of services found
 */
int AutoDiscovery::queryService(const std::string &service, const std::string &protocol)
{
    int n = MDNS.queryService(service.c_str(), protocol.c_str());
    if (n == 0)
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
    log_i("%d services found", n);
    for (int i = 0; i < n; ++i)
    {
        // print details for each service found
        log_i("service #%d: %s - %s:%d", i + 1, MDNS.hostname(i).c_str(), MDNS.IP(i).toString().c_str(), MDNS.port(i));
    }
    return n;
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