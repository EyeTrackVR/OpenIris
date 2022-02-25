#include <Arduino.h>
#include "pinout.h"

void setupCamera();
void setupWifi();
void startStreamServer();

void setup(){
    Serial.begin(115200);
    Serial.setDebugOutput(true);
    Serial.println();

    pinMode(LED_INDICATOR, OUTPUT);

    setupCamera();
    setupWifi();
    startStreamServer();
    
    // for indicating that everything is working
    digitalWrite(LED_INDICATOR, HIGH);
}

void loop(){
    delay(1);
}