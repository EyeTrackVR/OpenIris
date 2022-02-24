#include <Arduino.h>

void setupCamera();
void setupWifi();
void startStreamServer();

void setup(){
    Serial.begin(115200);
    Serial.setDebugOutput(true);
    Serial.println();

    setupCamera();
    setupWifi();
    startStreamServer();
}

void loop(){
    delay(1);
}