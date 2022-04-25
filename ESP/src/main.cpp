#include <Arduino.h>
#include "pinout.h"
#include "credentials.h"
#include "WifiHandler.h"
#include "MDNSManager.h"
#include "cameraHandler.h"
#include "LEDManager.h"
#include "webServer/webserverHandler.h"
#include "OTA.h"
#include "StateManager.h"

char *MDSNTrackerName = "OpenIrisTracker";
int STREAM_SERVER_PORT = 80;

auto ota = OpenIris::OTA();
auto ledManager = OpenIris::LEDManager(33);
auto cameraHandler = OpenIris::CameraHandler();
auto stateManager = OpenIris::StateManager();
auto httpdHandler = OpenIris::HTTPDHandler();

void setup()
{
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();
  ledManager.setupLED();
  cameraHandler.setupCamera();
  OpenIris::WiFiHandler::setupWifi(ssid, password);
  OpenIris::MDNSHandler::setupMDNS();
  httpdHandler.startStreamServer();
  ledManager.on();

  ota.SetupOTA(OTAPassword, OTAServerPort);
}

void loop()
{
  ota.HandleOTAUpdate();
  ledManager.displayStatus();
}