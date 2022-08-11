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
// Make unique is a templated function that takes a class and returns a unique pointer to that class -
// it is used to create a unique pointer to a class and ensure exception safety
std::unique_ptr<ProjectConfig> deviceConfig = std::make_unique<ProjectConfig>();
OTA ota(&*deviceConfig);
std::unique_ptr<SerialManager> serialManager = std::make_unique<SerialManager>(&*deviceConfig);
std::unique_ptr<WiFiHandler> wifiHandler = std::make_unique<WiFiHandler>(&*deviceConfig, &wifiStateManager);
std::unique_ptr<LEDManager> ledManager = std::make_unique<LEDManager>(33);
std::shared_ptr<CameraHandler> cameraHandler = std::make_shared<CameraHandler>(&*deviceConfig);           //! Create a shared pointer to the camera handler
std::unique_ptr<APIServer> apiServer = std::make_unique<APIServer>(CONTROL_SERVER_PORT, &*cameraHandler); //! Dereference the shared pointer to get the address of the camera handler
std::unique_ptr<MDNSHandler> mdnsHandler = std::make_unique<MDNSHandler>(&mdnsStateManager, &*deviceConfig);
std::unique_ptr<StreamServer> streamServer = std::make_unique<StreamServer>(STREAM_SERVER_PORT);

void setup()
{
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  ledManager->begin();
  deviceConfig->initConfig();
  deviceConfig->load();
  cameraHandler->setupCamera();

  wifiHandler->setupWifi();
  mdnsHandler->startMDNS();

  switch (wifiStateManager.getCurrentState())
  {
  case ProgramStates::DeviceStates::WiFiState_e::WiFiState_Disconnected:
  {
    break;
  }
  case ProgramStates::DeviceStates::WiFiState_e::WiFiState_Disconnecting:
  {
    break;
  }
  case ProgramStates::DeviceStates::WiFiState_e::WiFiState_ADHOC:
  {
  }
  case ProgramStates::DeviceStates::WiFiState_e::WiFiState_Connected:
  {
    apiServer->startAPIServer();
    streamServer->startStreamServer();
    log_d("[SETUP]: Starting Stream Server");
    break;
  }
  case ProgramStates::DeviceStates::WiFiState_e::WiFiState_Connecting:
  {
    break;
  }
  case ProgramStates::DeviceStates::WiFiState_e::WiFiState_Error:
  {
    break;
  }
  }
  ota.SetupOTA();
}

void loop()
{
  ota.HandleOTAUpdate();
  ledManager->displayStatus();
  serialManager->handleSerial();
}