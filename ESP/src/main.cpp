#include <Arduino.h>
#include <data/utilities/makeunique.hpp>
#include <network/WifiHandler/WifiHandler.hpp>
#include <network/mDNS/MDNSManager.hpp>
#include <io/camera/cameraHandler.hpp>
#include <io/LEDManager/LEDManager.hpp>
#include <network/stream/streamServer.hpp>
#include <network/api/webserverHandler.hpp>
#include <logo/logo.hpp>
#include <data/config/project_config.hpp>
//#include <io/SerialManager/serialmanager.hpp> // Serial Manager

#include <network/OTA/OTA.hpp>

int STREAM_SERVER_PORT = 80;
int CONTROL_SERVER_PORT = 81;

ProjectConfig deviceConfig;
OTA ota(&deviceConfig);
LEDManager ledManager(33);
CameraHandler cameraHandler(&deviceConfig);
// SerialManager serialManager(&deviceConfig);
WiFiHandler wifiHandler(&deviceConfig, &wifiStateManager, WIFI_SSID, WIFI_PASSWORD, WIFI_CHANNEL);
APIServer apiServer(CONTROL_SERVER_PORT, &deviceConfig, &cameraHandler, &wifiStateManager, "/control");
MDNSHandler mdnsHandler(&mdnsStateManager, &deviceConfig);
StreamServer streamServer(STREAM_SERVER_PORT);

void setup()
{
	Serial.begin(115200);
	Serial.setDebugOutput(DEBUG_MODE);
	Serial.println("\n");
	Logo::printASCII();

	ledManager.begin();

	deviceConfig.attach(&cameraHandler);
	deviceConfig.attach(&mdnsHandler);

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
		//! TODO: Implement
		break;
	}
	case WiFiState_e::WiFiState_Disconnecting:
	{
		//! TODO: Implement
		break;
	}
	case WiFiState_e::WiFiState_ADHOC:
	{
		streamServer.startStreamServer();
		log_d("[SETUP]: Starting Stream Server");
		apiServer.begin();
		log_d("[SETUP]: Starting API Server");
		break;
	}
	case WiFiState_e::WiFiState_Connected:
	{
		streamServer.startStreamServer();
		log_d("[SETUP]: Starting Stream Server");
		apiServer.begin();
		log_d("[SETUP]: Starting API Server");
		break;
	}
	case WiFiState_e::WiFiState_Connecting:
	{
		//! TODO: Implement
		break;
	}
	case WiFiState_e::WiFiState_Error:
	{
		//! TODO: Implement
		break;
	}
	}
	ota.SetupOTA();
}

void loop()
{
	ota.HandleOTAUpdate();
	ledManager.displayStatus();
	//  serialManager.handleSerial();
}