# include "LEDManager.h"

namespace LEDManager {
    uint8_t ledPin = 33;

    void setupLED(){
        pinMode(ledPin, OUTPUT);
        off();
    }

    void on(){
        digitalWrite(ledPin, LOW);
    }

    void off(){
        digitalWrite(ledPin, HIGH);
    }

    void blink(unsigned int time){
        on();
        delay(time);
        off();
        delay(time);
    }

    void displayPattern(Status status){
        if(status == Status::ConnectingToWifi){
            blink(1600);
            delay(1600);
        }
        if(status == Status::ConnectingToWifiError){
            for (int i = 0; i < 5; i++){
                blink(1000);
                delay(1000);
            }
        }
        if(status == Status::ConnectingToWifiSuccess){
            for (int i = 0; i < 2; i++){
                blink(1600);
                delay(1600);
            }
        }
    }
}