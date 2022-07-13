#include "MDNSManager.h"
#include "GlobalVars.h"

void MDNSHandler::setupMDNS()
{
  if (MDNS.begin(MDSNTrackerName))
  {
    stateManager.setState(State::MDNSSuccess); // todo make it a separate state from the global one
    MDNS.addService("openIrisTracker", "tcp", 80);
    MDNS.addServiceTxt("openIrisTracker", "tcp", "stream_port", String(80));
    log_d("MDNS initialized!");
  }
  else
  {
    stateManager.setState(State::MDNSError);
    log_e("Error initializing MDNS");
  }
}