#include "MDNSManager.h"
#include "GlobalVars.h"

void OpenIris::MDNSHandler::setupMDNS()
{
  if (MDNS.begin(MDSNTrackerName))
  {
    stateManager.setState(OpenIris::State::MDNSSuccess);
    MDNS.addService("openIrisTracker", "tcp", 80);
    MDNS.addServiceTxt("openIrisTracker", "tcp", "stream_port", String(80));
    log_d("MDNS initialized!");
  }
  else
  {
    stateManager.setState(OpenIris::State::MDNSError);
    log_e("Error initializing MDNS");
  }
}