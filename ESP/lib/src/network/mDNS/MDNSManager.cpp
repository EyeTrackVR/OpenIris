#include "MDNSManager.hpp"

void MDNSHandler::setupMDNS(const char *trackerName, StateManager<ProgramStates::DeviceStates::MDNSState_e> *stateManager)
{
    if (MDNS.begin(trackerName))
    {
        stateManager->setState(ProgramStates::DeviceStates::MDNSState_e::MDNSState_Started);
        MDNS.addService("openIrisTracker", "tcp", 80);
        MDNS.addServiceTxt("openIrisTracker", "tcp", "stream_port", String(80));
        log_d("MDNS initialized!");
    }
    else
    {
        stateManager->setState(ProgramStates::DeviceStates::MDNSState_e::MDNSState_Error);
        log_e("Error initializing MDNS");
    }
}