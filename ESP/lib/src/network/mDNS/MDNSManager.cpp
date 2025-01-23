#include "MDNSManager.hpp"

MDNSHandler::MDNSHandler(ProjectConfig& configManager)
    : configManager(configManager) {}

bool MDNSHandler::startMDNS() {
  const std::string service = "_openiristracker";
  auto mdnsConfig = configManager.getMDNSConfig();
  if (!MDNS.begin(mdnsConfig.hostname
                      .c_str()))  // lowercase only - as this will be the url
  {
    mdnsStateManager.setState(MDNSState_e::MDNSState_Error);
    log_e("Error initializing MDNS");
    return false;
  }

  mdnsStateManager.setState(MDNSState_e::MDNSState_Starting);
  MDNS.addService(service.c_str(), "_tcp", 80);
  char port[20];
  //! Add service needs leading _ on ESP32 implementation for some reason
  //! (according to the docs)
  MDNS.addServiceTxt(service.c_str(), "_tcp", "stream_port", "80");
  MDNS.addServiceTxt(service.c_str(), "_tcp", "api_port", "81");
  log_i("MDNS initialized!");
  mdnsStateManager.setState(MDNSState_e::MDNSState_Started);
  return true;
}

std::string MDNSHandler::getName() {
  return "MDNSHandler";
}

void MDNSHandler::update(ConfigState_e event) {
  switch (event) {
    case ConfigState_e::mdnsConfigUpdated:
      MDNS.end();
      startMDNS();
      break;
    default:
      break;
  }
}
