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

StateManager<ProgramStates::DeviceStates::MDNSState_e> mdns_stateManager;
StateManager<ProgramStates::DeviceStates::WiFiState_e> wifi_stateManager;
StateManager<ProgramStates::DeviceStates::WebServerState_e> web_stateManager;
StateManager<ProgramStates::DeviceStates::CameraState_e> camera_stateManager;
StateManager<ProgramStates::DeviceStates::ButtonState_e> button_stateManager;
StateManager<ProgramStates::DeviceStates::StreamState_e> stream_stateManager;

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

  WiFiHandler::setupWifi(ssid, password, &wifi_stateManager);
  MDNSHandler::setupMDNS(MDSNTrackerName, &mdns_stateManager);
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