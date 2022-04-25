#include "MDNSManager.h"
#include "GlobalVars.h"

void OpenIris::MDNSHandler::setupMDNS()
{
  if (MDNS.begin(MDSNTrackerName))
  {
    stateManager.setState(OpenIris::State::MDNSSuccess);
    MDNS.addService("openIrisTracker", "tcp", 80);
    MDNS.addServiceTxt("openIrisTracker", "tcp", "stream_port", String(80));
    Serial.println("MDNS initialized!");
  }
  else
  {
    stateManager.setState(OpenIris::State::MDNSError);
    Serial.println("Error initializing MDNS");
  }
}