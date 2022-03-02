#include <Arduino.h>
#include "pinout.h"
#include "credentials.h"
#include "WifiHandler.h"
#include "cameraHandler.h"
#include "LEDManager.h"
#include "httpdHandler.h"
#include "OTA.h"

void setup(){
    Serial.begin(115200);
    Serial.setDebugOutput(true);
    Serial.println();
    
    Serial.println("setting up led");
    LEDManager::setupLED();
    // todo add blink handling
    CameraHandler::setupCamera();
    WiFiHandler::setupWifi(ssid, password);
    // todo add blink handling
    HttpdHandler::startStreamServer();
    LEDManager::on();

    OTA::SetupOTA(OTAPassword, OTAServerPort);
}

void loop(){
    OTA::HandleOTAUpdate();
}