#include "MDNSManager.hpp"

void MDNSHandler::startMDNS()
{
  ProjectConfig::DeviceConfig_t *deviceConfig = configManager->getDeviceConfig();

  if (MDNS.begin(deviceConfig->name))
  {
    stateManager->setState(MDNSState_e::MDNSState_Starting);
    MDNS.addService("openIrisTracker", "tcp", 80);
    MDNS.addServiceTxt("openIrisTracker", "tcp", "stream_port", String(80));
    log_i("MDNS initialized!");
    stateManager->setState(MDNSState_e::MDNSState_Started);
  }
  else
  {
    stateManager->setState(MDNSState_e::MDNSState_Error);
    log_e("Error initializing MDNS");
  }
}

void MDNSHandler::update(ObserverEvent::Event event)
{
  if (event == ObserverEvent::deviceConfigUpdated)
  {
    MDNS.end();
    startMDNS();
  }
}