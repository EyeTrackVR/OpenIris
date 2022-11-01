#include "MDNSManager.hpp"

MDNSHandler::MDNSHandler(StateManager<MDNSState_e> *stateManager,
						 ProjectConfig *configManager) : stateManager(stateManager),
														 configManager(configManager) {}

void MDNSHandler::startMDNS()
{
	ProjectConfig::DeviceConfig_t *deviceConfig = configManager->getDeviceConfig();
	// deviceConfig->name.c_str()
	if (MDNS.begin("openiristracker")) // lowercase only - as this will be the url
	{
		stateManager->setState(MDNSState_e::MDNSState_Starting);
		MDNS.addService(deviceConfig->name.c_str(), "tcp", 80);
		char port[20];
		//! Add service needs leading _ on ESP32 implementation for some reason (according to the docs)
		MDNS.addServiceTxt(("_" + deviceConfig->name).c_str(), "_tcp", "_stream_port", (const char *)Helpers::itoa(80, port, 10)); //! convert int to const char* using a very efficient implemenation of itoa
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
	case ObserverEvent::Event::deviceConfigUpdated:
		MDNS.end();
		startMDNS();
		break;
	default:
		break;
	}
}