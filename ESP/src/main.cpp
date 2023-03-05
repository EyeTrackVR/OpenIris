#include <Arduino.h>
#include <network/WifiHandler/WifiHandler.hpp>
#include <network/mDNS/MDNSManager.hpp>
#include <io/camera/cameraHandler.hpp>
#include <io/LEDManager/LEDManager.hpp>
#include <network/stream/streamServer.hpp>
#include <network/api/webserverHandler.hpp>

//! TODO: Setup OTA enabled state to be controllable by API if enabled at compile time
#if ENABLE_OTA
#include <network/OTA/OTA.hpp>
#endif // ENABLE_OTA
#include <logo/logo.hpp>
#include <data/config/project_config.hpp>

int STREAM_SERVER_PORT = 80;
int CONTROL_SERVER_PORT = 81;
/**
 * @brief ProjectConfig object
 * @brief This is the main configuration object for the project
 * @param name The name of the project config partition
 * @param mdnsName The mDNS hostname to use
 */
ProjectConfig deviceConfig("openiris", MDNS_HOSTNAME);

#if ENABLE_OTA
OTA ota(&deviceConfig);
#endif // ENABLE_OTA
LEDManager ledManager(33);
CameraHandler cameraHandler(&deviceConfig, &ledStateManager);

WiFiHandler wifiHandler(&deviceConfig, &wifiStateManager, WIFI_SSID, WIFI_PASSWORD, WIFI_CHANNEL);
APIServer apiServer(CONTROL_SERVER_PORT, &deviceConfig, &cameraHandler, &wifiStateManager, "/control");
MDNSHandler mdnsHandler(&mdnsStateManager, &deviceConfig);
StreamServer streamServer(STREAM_SERVER_PORT, &wifiStateManager);

void setup()
{
	setCpuFrequencyMhz(240); // set to 240mhz for performance boost
	Serial.begin(115200);
	//Serial.setDebugOutput(DEBUG_MODE);
	//Serial.println("Free Heap: " + String(ESP.getFreeHeap()));
	Logo::printASCII();
	Serial.flush();
	//Serial.println("Free Heap: " + String(ESP.getFreeHeap()));

	ledManager.begin();
	deviceConfig.attach(&cameraHandler);
	deviceConfig.attach(&mdnsHandler);
	deviceConfig.initConfig();
	deviceConfig.load();
	wifiHandler._enable_adhoc = ENABLE_ADHOC;
	wifiHandler.setupWifi();
    mdnsHandler.startMDNS();

    /* mdnsStateManager.setState(MDNSState_e::MDNSState_Starting);
	switch (mdnsStateManager.getCurrentState())
	{
	case MDNSState_e::MDNSState_Starting:
		break;
	case MDNSState_e::MDNSState_Error:
		break;
	case MDNSState_e::MDNSState_QueryComplete:
		mdnsHandler.startMDNS();
		break;
	default:
		break;
	} */

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
#if ENABLE_OTA
	ota.begin();
#endif // ENABLE_OTA
}

void loop()
{
#if ENABLE_OTA
	ota.handleOTAUpdate();
#endif // ENABLE_OTA
	ledManager.handleLED(&ledStateManager);
}
