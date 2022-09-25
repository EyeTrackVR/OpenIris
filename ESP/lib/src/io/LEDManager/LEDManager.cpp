#include "LEDManager.hpp"

LEDManager::LEDManager(byte pin) : _ledPin(pin), _previousMillis(0), _ledState(false) {}

LEDManager::~LEDManager() {}

void LEDManager::begin()
{
	pinMode(_ledPin, OUTPUT);
	onOff(false);
}

/**
 * @brief Control the LED timer
 * @details This function must be called in the main loop
 * 
 * @param time 
 */
void LEDManager::handleLED(unsigned long time)
{
	unsigned long currentMillis = millis();
	if (currentMillis - _previousMillis >= time)
	{
		_previousMillis = currentMillis;
		_ledState = !_ledState;
	}
}

/**
 * @brief Turn the LED on or off
 * 
 * @param state 
 */
void LEDManager::onOff(bool state) const
{
	digitalWrite(_ledPin, state);
}

/**
 * @brief Blink the LED
 * @details This function requires the handleLED function to be called in the main loop
 */
void LEDManager::blink()
{
	onOff(_ledState);
}

/**
 * @brief Blink the LED a number of times
 * @details This function is blocking and does not require the handleLED function to be called in the main loop
 * @param times 
 * @param delayTime 
 */
void LEDManager::blink(int times, int delayTime)
{
	for (int i = 0; i < times; i++)
	{
		onOff(true);
		delay(delayTime);
		onOff(false);
		delay(delayTime);
	}
}

/**
 * @brief Display the status of the LED
 * @details This function requires the handleLED function to be called in the main loop
 */
void LEDManager::displayStatus()
{
}
