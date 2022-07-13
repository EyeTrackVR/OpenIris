#include <Arduino.h>
#include "pinout.h"
#include "credentials.h"
#include "WifiHandler.h"
#include "MDNSManager.h"
#include "cameraHandler.h"
#include "LEDManager.h"
#include "streamServer.h"
#include "webserverHandler.h"

#include "OTA.h"
#include "StateManager.h"

const char *MDSNTrackerName = "OpenIrisTracker";

int STREAM_SERVER_PORT = 80;
int CONTROL_SERVER_PORT = 81;

auto ota = OpenIris::OTA();
auto ledManager = OpenIris::LEDManager(33);
auto cameraHandler = OpenIris::CameraHandler();
auto stateManager = OpenIris::StateManager();
auto apiServer = OpenIris::APIServer();
auto streamServer = OpenIris::StreamServer();

void setup()
{
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  ledManager.setupLED();
  cameraHandler.setupCamera();
  OpenIris::WiFiHandler::setupWifi(ssid, password);
  OpenIris::MDNSHandler::setupMDNS();
  apiServer.startAPIServer();
  streamServer.startStreamServer();
  ledManager.on();

  ota.SetupOTA(OTAPassword, OTAServerPort);
}

void loop()
{
  ota.HandleOTAUpdate();
  ledManager.displayStatus();
}