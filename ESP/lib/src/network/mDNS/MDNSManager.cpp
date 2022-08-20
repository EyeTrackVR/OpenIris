#include "MDNSManager.hpp"

void MDNSHandler::startMDNS()
{
	ProjectConfig::DeviceConfig_t *deviceConfig = configManager->getDeviceConfig();

	if (MDNS.begin(deviceConfig->name.c_str()))
	{
		stateManager->setState(MDNSState_e::MDNSState_Starting);
		MDNS.addService("openIrisTracker", "tcp", 80);
		char port[20];
		//! Add service needs leading _ on ESP32 implementation for some reason (according to the docs)
		MDNS.addServiceTxt("_openIrisTracker", "_tcp", "_stream_port", (const char *)Helpers::itoa(80, port, 10)); //! convert int to const char* using a very efficient implemenation of itoa
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