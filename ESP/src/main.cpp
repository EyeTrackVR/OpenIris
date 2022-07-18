#include <Arduino.h>
#include <network/WifiHandler/WifiHandler.hpp>
#include <network/mDNS/MDNSManager.hpp>
#include <io/camera/cameraHandler.hpp>
#include <io/LEDManager/LEDManager.hpp>
#include <network/stream/streamServer.hpp>
#include <network/webserver/webserverHandler.hpp>

#include <network/OTA/OTA.hpp>

int STREAM_SERVER_PORT = 80;
int CONTROL_SERVER_PORT = 81;

OTA ota;
LEDManager ledManager(33);
CameraHandler cameraHandler;
APIServer apiServer(CONTROL_SERVER_PORT, &cameraHandler);
StreamServer streamServer(STREAM_SERVER_PORT);

//******************************************************************************
//                      * Welcome to OpenIris! 
//            * Please make sure you have the following:
//              * 1. A camera connected to the ESP32
//              * 2. Your wifi credentials saved in the platformio.ini file
//              * 3. You have downloaded our app and installed it on your PC
// ******************************************************************************
void setup()
{
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  ledManager.begin();
  cameraHandler.setupCamera();

  WiFiHandler::setupWifi(WIFI_SSID, WIFI_PASSWORD, &wifiStateManager);
  MDNSHandler::setupMDNS(MDNS_TRACKER_NAME, &mdnsStateManager);
  apiServer.startAPIServer();
  streamServer.startStreamServer();
  ledManager.onOff(true);

  ota.SetupOTA(OTA_PASSWORD, OTA_SERVER_PORT);
}

void loop()
{
  ota.HandleOTAUpdate();
  ledManager.displayStatus();
}