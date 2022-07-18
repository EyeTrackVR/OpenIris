#include <Arduino.h>
#include "pinout.h"
#include "credentials.h"
#include <network/WifiHandler/WifiHandler.hpp>
#include <network/mDNS/MDNSManager.hpp>
#include <io/camera/cameraHandler.hpp>
#include <io/LEDManager/LEDManager.hpp>
#include <network/stream/streamServer.hpp>
#include <network/webserver/webserverHandler.hpp>

#include <network/OTA/OTA.hpp>



const char *MDSNTrackerName = "OpenIrisTracker";

int STREAM_SERVER_PORT = 80;
int CONTROL_SERVER_PORT = 81;

StateManager<ProgramStates::DeviceStates::State_e> stateManager;
StateManager<ProgramStates::DeviceStates::WiFiState_e> wifiStateManager;
StateManager<ProgramStates::DeviceStates::WebServerState_e> webServerStateManager;
StateManager<ProgramStates::DeviceStates::MDNSState_e> mdnsStateManager;
StateManager<ProgramStates::DeviceStates::CameraState_e> cameraStateManager;
StateManager<ProgramStates::DeviceStates::ButtonState_e> buttonStateManager;
StateManager<ProgramStates::DeviceStates::StreamState_e> streamStateManager;

OTA ota;
LEDManager ledManager(33);
CameraHandler cameraHandler;
APIServer apiServer(CONTROL_SERVER_PORT, &cameraHandler);
StreamServer streamServer(STREAM_SERVER_PORT);

void setup()
{
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  ledManager.begin();
  cameraHandler.setupCamera();

  WiFiHandler::setupWifi(ssid, password, &wifiStateManager);
  MDNSHandler::setupMDNS(MDSNTrackerName, &mdnsStateManager);
  apiServer.startAPIServer();
  streamServer.startStreamServer();
  ledManager.onOff(true);

  ota.SetupOTA(OTAPassword, OTAServerPort);
}

void loop()
{
  ota.HandleOTAUpdate();
  ledManager.displayStatus();
}