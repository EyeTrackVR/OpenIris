#include <WiFi.h>
#include "credentials.h"

void setupWifi(){
    Serial.println("Initializing connection to wifi");

    WiFi.begin(ssid, password);

    Serial.print("connecting");
    while (WiFi.status() != WL_CONNECTED){
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.print("ESP will be streaming under 'http://");
    Serial.print(WiFi.localIP());
    Serial.print("/\r\n");
}