#include "LEDManager.hpp"

LEDManager::LEDManager(byte pin) : _ledPin(pin), _previousMillis(0), _ledState(false) {}

LEDManager::~LEDManager() {}


void LEDManager::begin()
{
    pinMode(_ledPin, OUTPUT);
    onOff(false);

    /* for (auto &led : _leds)
    {
        if (led > 0)
        {
            pinMode(led, OUTPUT);
        }
    } */
}


void LEDManager::onOff(bool state) const
{
    digitalWrite(_ledPin, state);
}

void LEDManager::blink(unsigned long time)
{
    unsigned long currentMillis = millis();
    if (currentMillis - _previousMillis >= time)
    {
        _previousMillis = currentMillis;
        _ledState = !_ledState;
        onOff(_ledState);
    }
}

void LEDManager::displayStatus()
{
}
