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

ProjectConfig deviceConfig;
OTA ota(&deviceConfig);
LEDManager ledManager(33);
CameraHandler cameraHandler(&deviceConfig);
// SerialManager serialManager(&deviceConfig);

APIServer apiServer(81, &deviceConfig, &cameraHandler, &wifiStateManager, "/control");
StreamServer streamServer(80);

WiFiHandler wifiHandler(&deviceConfig, &apiServer, &streamServer, &wifiStateManager, WIFI_SSID, WIFI_PASSWORD, WIFI_CHANNEL);
MDNSHandler mdnsHandler(&mdnsStateManager, &deviceConfig);

void setup()
{
	Serial.begin(115200);
	Serial.setDebugOutput(DEBUG_MODE);
	Serial.println("\n");
	Logo::printASCII();

	ledManager.begin();
	deviceConfig.initConfig();
	deviceConfig.load();
	cameraHandler.setupCamera();

	wifiHandler._enable_adhoc = ENABLE_ADHOC; // force ADHOC mode at compile time.
	wifiHandler.begin(); // start wifi, apiserver, and streamserver
	mdnsHandler.startMDNS();
	ota.SetupOTA();
}

void loop()
{
	ota.HandleOTAUpdate();
	ledManager.displayStatus();
	//  serialManager.handleSerial();
}