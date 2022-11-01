#include "MDNSManager.hpp"

MDNSHandler::MDNSHandler(StateManager<MDNSState_e> *stateManager,
						 ProjectConfig *configManager) : stateManager(stateManager),
														 configManager(configManager) {}

void MDNSHandler::startMDNS()
{
	ProjectConfig::MDNSConfig_t *mdnsConfig = configManager->getMDNSConfig();
	if (MDNS.begin(mdnsConfig->hostname.c_str())) // lowercase only - as this will be the url
	{
		stateManager->setState(MDNSState_e::MDNSState_Starting);
		MDNS.addService(mdnsConfig->hostname.c_str(), "tcp", 80);
		char port[20];
		//! Add service needs leading _ on ESP32 implementation for some reason (according to the docs)
		MDNS.addServiceTxt(("_" + mdnsConfig->hostname).c_str(), "_tcp", "_stream_port", (const char *)Helpers::itoa(80, port, 10)); //! convert int to const char* using a very efficient implemenation of itoa
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
	switch (event)
	{
	case ObserverEvent::Event::mdnsConfigUpdated:
		MDNS.end();
		startMDNS();
		break;
	default:
		break;
	}
}