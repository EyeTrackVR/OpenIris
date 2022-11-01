#include "queryMDNS.hpp"

QueryMDNSService::QueryMDNSService(StateManager<MDNSState_e> *stateManager,
                                   ProjectConfig *configManager) : stateManager(stateManager),
                                                                   configManager(configManager) {}

QueryMDNSService::~QueryMDNSService()
{
    // Finalize the MDNS library
    mdns_free();
}

void QueryMDNSService::queryMDNS()
{
    stateManager->setState(MDNSState_e::MDNSState_QueryComplete);
    ProjectConfig::MDNSConfig_t *mdnsConfig = configManager->getMDNSConfig();
    // check if we have a valid MDNS config
    if (mdnsConfig->hostname.empty() && mdnsConfig->service.empty())
    {
        // Initialize the MDNS library
        std::string hostname;
        std::string service;
        // Initialize the MDNS library
        if (mdns_init() != ESP_OK)
        {
            log_e("Error initializing MDNS Query");
            return;
        }
        int services_found = MDNS.queryService("openiristracker", "tcp");

        if (services_found == 0)
        {
            log_e("No services found!");
            log_e("Setting a default hostname of 'openiristracker'");
            hostname.assign("openiristracker");
            log_i("Hostname: %s", hostname.c_str());
            service.assign("openiristracker");
        }
        else
        {
            log_i("Services found: %d", services_found);
            for (int i = 0; i < services_found; i++)
            {
                log_i("------------------------");
                log_i("Service #%d", i);
                log_i(" - Hostname: %s", MDNS.hostname(i).c_str());
                log_i(" - IP: %s", MDNS.IP(i).toString().c_str());
                log_i(" - Port: %d", MDNS.port(i));
                log_i("------------------------");
            }
            // append one greater than the number of services found to the hostname
            // this will allow for multiple trackers to be found
            hostname.assign(MDNS.hostname(services_found).c_str());
            hostname.append(std::to_string(services_found));

            service.assign(MDNS.hostname(services_found).c_str());
            service.append(std::to_string(services_found));
        }
        mdns_free(); // Free the MDNS library
        stateManager->setState(MDNSState_e::MDNSState_QueryComplete);
        return;
    }
    stateManager->setState(MDNSState_e::MDNSState_QueryComplete);
    return;
}