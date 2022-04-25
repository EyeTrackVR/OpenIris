#include "LEDManager.h"

void OpenIris::LEDManager::setupLED() const
{
  pinMode(ledPin, OUTPUT);
}

void OpenIris::LEDManager::on() const
{
  digitalWrite(ledPin, LOW);
}

void OpenIris::LEDManager::off() const
{
  digitalWrite(ledPin, HIGH);
}

void OpenIris::LEDManager::blink(unsigned int time)
{
  on();
  delay(time);
  off();
}

void OpenIris::LEDManager::displayStatus()
{
}
