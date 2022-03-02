#include "WifiHandler.h" 
#include "LEDManager.h"

namespace WiFiHandler {
    void setupWifi(const char* ssid, const char* password){
        Serial.println("Initializing connection to wifi");

        WiFi.begin(ssid, password);

        Serial.print("connecting");
        int time_spent_connecting = 0;
        int connection_timeout = 6400;
        int wifi_status = WiFi.status();

        while (time_spent_connecting < connection_timeout || wifi_status != WL_CONNECTED){
            wifi_status = WiFi.status();
            Serial.print(".");
            LEDManager::blink(LEDManager::Status::ConnectingToWifi);
            time_spent_connecting += 1600;
            delay(1600);
        }

        if(wifi_status == WL_CONNECTED){
            LEDManager::blink(LEDManager::Status::ConnectingToWifiSuccess);
            delay(1600);
            Serial.print("\n\rWiFi connected\n\r");
            Serial.print("ESP will be streaming under 'http://");
            Serial.print(WiFi.localIP());
            Serial.print(":81/\r\n");
            Serial.print("ESP will be accepting commands under 'http://");
            Serial.print(WiFi.localIP());
            Serial.print(":80/control\r\n");
        }
        else{
            LEDManager::blink(LEDManager::Status::ConnectingToWifiError);
            return;
        }
    }
}