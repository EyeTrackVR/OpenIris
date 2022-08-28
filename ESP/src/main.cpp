#include <Arduino.h>
#include <data/utilities/makeunique.hpp>
#include <network/WifiHandler/WifiHandler.hpp>
#include <network/mDNS/MDNSManager.hpp>
#include <io/camera/cameraHandler.hpp>
#include <io/LEDManager/LEDManager.hpp>
#include <network/stream/streamServer.hpp>
#include <network/api/webserverHandler.hpp>
#include <data/config/project_config.hpp>
//#include <io/SerialManager/serialmanager.hpp> // Basic Serial Manager
//#include <io/SerialManager/SerialManager2/serialmanager.hpp  // Advanced Serial MAnager //! Finish this to update the serial manager

#include <network/OTA/OTA.hpp>

int STREAM_SERVER_PORT = 80;
int CONTROL_SERVER_PORT = 81;

ProjectConfig deviceConfig;
OTA ota(&deviceConfig);
LEDManager ledManager(33);
CameraHandler cameraHandler(&deviceConfig);
// SerialManager serialManager(&deviceConfig);
WiFiHandler wifiHandler(&deviceConfig, &wifiStateManager, WIFI_SSID, WIFI_PASSWORD, 1);
APIServer apiServer(CONTROL_SERVER_PORT, &wifiHandler, &cameraHandler, &wifiStateManager, "/control");
MDNSHandler mdnsHandler(&mdnsStateManager, &deviceConfig);
StreamServer streamServer(STREAM_SERVER_PORT);

void setup()
{
	Serial.begin(115200);
	Serial.setDebugOutput(true);
	Serial.println("\n");
	API_Utilities::printASCII();

	ledManager.begin();
	deviceConfig.initConfig();
	deviceConfig.load();
	cameraHandler.setupCamera();

	wifiHandler._enable_adhoc = ENABLE_ADHOC;

	wifiHandler.setupWifi();
	mdnsHandler.startMDNS();

	switch (wifiStateManager.getCurrentState())
	{
	case WiFiState_e::WiFiState_Disconnected:
	{
		break;
	}
	case WiFiState_e::WiFiState_Disconnecting:
	{
		break;
	}
	case WiFiState_e::WiFiState_ADHOC:
	{
		streamServer.startStreamServer();
		apiServer.begin();
		log_d("[SETUP]: Starting Stream Server");
		break;
	}
	case WiFiState_e::WiFiState_Connected:
	{
		streamServer.startStreamServer();
		apiServer.begin();
		log_d("[SETUP]: Starting Stream Server");
		break;
	}
	case WiFiState_e::WiFiState_Connecting:
	{
		break;
	}
	case WiFiState_e::WiFiState_Error:
	{
		break;
	}
	}
	ota.SetupOTA();
}

void loop()
{
	ota.HandleOTAUpdate();
	ledManager.displayStatus();
	apiServer.triggerWifiConfigWrite();
	//  serialManager.handleSerial();
}