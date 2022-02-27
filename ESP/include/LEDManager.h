#include <Arduino.h>

namespace LEDManager{

    enum Status {
        ConnectingToWifi = 1,
        ConnectingToWifiError = 2,
        ConnectingToWifiSuccess = 3,
        ServerError = 3,
        CameraError = 4
    };

    extern uint8_t ledPin;

    void setupLED();
    void on();
    void off();
    void blink(unsigned int time);
    void displayPattern(Status status);
}