#include <Arduino.h>
#include "pinout.h"
#include "WifiHandler.h"
#include "cameraHandler.h"
#include "LEDManager.h"
#include "httpdHandler.h"

void setup(){
    Serial.begin(115200);
    Serial.setDebugOutput(true);
    Serial.println();
    
    Serial.println("setting up led");
    LEDManager::setupLED();
    // todo add blink handling
    CameraHandler::setupCamera();
    WiFiHandler::setupWifi();
    // todo add blink handling
    HttpdHandler::startStreamServer();
    LEDManager::on();
}

void loop(){
    delay(1);
}