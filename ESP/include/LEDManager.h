#pragma once
#include <Arduino.h>

class LEDManager
{
private:
  uint8_t ledPin;

public:
  explicit LEDManager(uint8_t pin) : ledPin(pin) {}

  void setupLED() const;
  void on() const;
  void off() const;
  void blink(unsigned int time);
  void displayStatus();
};
