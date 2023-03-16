#include "MDNSManager.hpp"

MDNSHandler::MDNSHandler(StateManager<MDNSState_e>* stateManager,
                         ProjectConfig* configManager)
    : stateManager(stateManager), configManager(configManager) {}

bool MDNSHandler::startMDNS() {
  const std::string service = "_openiristracker";
  ProjectConfig::MDNSConfig_t* mdnsConfig = configManager->getMDNSConfig();
  if (!MDNS.begin(mdnsConfig->hostname
                      .c_str()))  // lowercase only - as this will be the url
  {
    stateManager->setState(MDNSState_e::MDNSState_Error);
    log_e("Error initializing MDNS");
    return false;
  }

  stateManager->setState(MDNSState_e::MDNSState_Starting);
  MDNS.addService(service.c_str(), "_tcp", 80);
  char port[20];
  //! Add service needs leading _ on ESP32 implementation for some reason
  //! (according to the docs)
  MDNS.addServiceTxt(service.c_str(), "_tcp", "stream_port",
                     "80");
  log_i("MDNS initialized!");
  stateManager->setState(MDNSState_e::MDNSState_Started);
  return true;
}

void MDNSHandler::update(ObserverEvent::Event event) {
  switch (event) {
    case ObserverEvent::Event::mdnsConfigUpdated:
      MDNS.end();
      startMDNS();
      break;
    default:
      break;
  }
}
