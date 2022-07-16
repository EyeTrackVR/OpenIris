#include "MDNSManager.h"

void MDNSHandler::setupMDNS(const char *trackerName, StateManager *stateManager)
{
  // TODO make it have a separate state manager
  if (MDNS.begin(trackerName))
  {
    stateManager->setState(State::MDNSSuccess); // todo make it a separate state from the global one
    MDNS.addService("openIrisTracker", "tcp", 80);
    MDNS.addServiceTxt("openIrisTracker", "tcp", "stream_port", String(80));
    log_d("MDNS initialized!");
  }
  else
  {
    stateManager->setState(State::MDNSError);
    log_e("Error initializing MDNS");
  }
}