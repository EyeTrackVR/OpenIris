#include "LEDManager.h"

void LEDManager::setupLED() const
{
  pinMode(ledPin, OUTPUT);
}

void LEDManager::on() const
{
  digitalWrite(ledPin, LOW);
}

void LEDManager::off() const
{
  digitalWrite(ledPin, HIGH);
}

void LEDManager::blink(unsigned int time)
{
  on();
  delay(time);
  off();
}

void LEDManager::displayStatus()
{
}
