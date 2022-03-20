#include <Arduino.h>
#include "pinout.h"
#include "credentials.h"
#include "WifiHandler.h"
#include "cameraHandler.h"
#include "LEDManager.h"
#include "httpdHandler.h"
#include "OTA.h"
#include "StateManager.h"

auto ota = OpenIris::OTA();
auto ledManager = OpenIris::LEDManager(33);
auto cameraHandler = OpenIris::CameraHandler();
auto stateManager = OpenIris::StateManager();
auto httpdHandler = OpenIris::HTTPDHandler();

void setup(){
    Serial.begin(115200);
    Serial.setDebugOutput(true);
    Serial.println();

    ledManager.setupLED();
    cameraHandler.setupCamera();
    OpenIris::WiFiHandler::setupWifi(ssid, password);
    httpdHandler.startStreamServer();
    ledManager.on();

    ota.SetupOTA(OTAPassword, OTAServerPort);
}

void loop(){
    ota.HandleOTAUpdate();
    ledManager.displayStatus();
}