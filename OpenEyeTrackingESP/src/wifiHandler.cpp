#include <WiFi.h>
#include "credentials.h"
#include "pinout.h"

void setupWifi(){
    Serial.println("Initializing connection to wifi");

    WiFi.begin(ssid, password);

    Serial.print("connecting");
    while (WiFi.status() != WL_CONNECTED){
        digitalWrite(LED_INDICATOR, LOW);
        delay(800);
        digitalWrite(LED_INDICATOR, HIGH);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.print("ESP will be streaming under 'http://");
    Serial.print(WiFi.localIP());
    Serial.print(":81/\r\n");
    Serial.print("ESP will be accepting commands under 'http://");
    Serial.print(WiFi.localIP());
    Serial.print(":80/control\r\n");
}