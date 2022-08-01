#include <Arduino.h>
#include <data/utilities/makeunique.hpp>
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

uint8_t STREAM_SERVER_PORT = 80;
uint8_t CONTROL_SERVER_PORT = 81;

// Create smart pointers to the various classes that will be used in the program to make sure that they are deleted when the program ends
// This is done to make sure that the memory is freed when the program ends and we are not left with dangling pointers to memory that is no longer in use
// Make unique is a templated function that takes a class and returns a unique pointer to that class - it is used to create a unique pointer to a class and ensure exception safety
std::unique_ptr<OTA> ota = Utilities::make_unique<OTA>();
std::unique_ptr<LEDManager> ledManager = Utilities::make_unique<LEDManager>(33);
std::unique_ptr<CameraHandler> cameraHandler = Utilities::make_unique<CameraHandler>(&projectConfig);
std::unique_ptr<APIServer> apiServer = Utilities::make_unique<APIServer>(CONTROL_SERVER_PORT, &*cameraHandler);
std::unique_ptr<MDNSHandler> mdnsHandler = Utilities::make_unique<MDNSHandler>(&mdnsStateManager, &projectConfig);
std::unique_ptr<StreamServer> streamServer = Utilities::make_unique<StreamServer>(STREAM_SERVER_PORT);

void setup()
{
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  ledManager->begin();
  projectConfig.initStructures();
  projectConfig.load();
  cameraHandler->setupCamera();

  WiFiHandler::setupWifi(&wifiStateManager, &projectConfig);
  mdnsHandler->startMDNS();

  if (wifiStateManager.getCurrentState() == ProgramStates::DeviceStates::WiFiState_e::WiFiState_Connected)
  {
    apiServer->startAPIServer();
    streamServer->startStreamServer();
  }
  ota->SetupOTA(&projectConfig);
  
}

void loop()
{
  ota->HandleOTAUpdate();
  ledManager->displayStatus();
  serialManager.handleSerial(); 
}