#include <Arduino.h>
#include <network/WifiHandler/WifiHandler.hpp>
#include <network/mDNS/MDNSManager.hpp>
#include <io/camera/cameraHandler.hpp>
#include <io/LEDManager/LEDManager.hpp>
#include <network/stream/streamServer.hpp>
#include <network/webserver/webserverHandler.hpp>
#include <data/config/project_config.hpp>
#include <io/SerialManager/serialmanager.hpp> // Basic Serial Manager
//#include <io/SerialManager/SerialManager2/serialmanager.hpp>  // Advanced Serial MAnager //! Finish this to update the serial manager

#include <network/OTA/OTA.hpp>

int STREAM_SERVER_PORT = 80;
int CONTROL_SERVER_PORT = 81;

OTA ota;
LEDManager ledManager(33);
CameraHandler cameraHandler(&projectConfig);
APIServer apiServer(CONTROL_SERVER_PORT, &cameraHandler);
MDNSHandler mdnsHandler(&mdnsStateManager, &projectConfig);
StreamServer streamServer(STREAM_SERVER_PORT);

void setup()
{
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  ledManager.begin();
  projectConfig.initStructures();
  projectConfig.load();
  cameraHandler.setupCamera();

  WiFiHandler::setupWifi(&wifiStateManager, &projectConfig);
  mdnsHandler.startMDNS();

  if (wifiStateManager.getCurrentState() == ProgramStates::DeviceStates::WiFiState_e::WiFiState_Connected)
  {
    apiServer.startAPIServer();
    streamServer.startStreamServer();
  }
  ota.SetupOTA(&projectConfig);
  
}

void loop()
{
  ota.HandleOTAUpdate();
  ledManager.displayStatus();
  serialManager.handleSerial(); 
}