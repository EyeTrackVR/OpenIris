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

auto ota = OTA();
auto stateManager = StateManager();
auto ledManager = LEDManager(33);
auto cameraHandler = CameraHandler();
auto apiServer = APIServer(CONTROL_SERVER_PORT, &cameraHandler);
auto streamServer = StreamServer(STREAM_SERVER_PORT);

void setup()
{
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  ledManager.setupLED();
  cameraHandler.setupCamera();
  WiFiHandler::setupWifi(ssid, password, &stateManager);
  MDNSHandler::setupMDNS(MDSNTrackerName, &stateManager);
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