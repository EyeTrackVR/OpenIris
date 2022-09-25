#include "LEDManager.hpp"

/**
 *! @brief This is declared as a static member of the class - therefor it must be initialized outside of the class.
 ** @brief This is a map of the LEDStates and the BlinkPatterns.
 ** @brief The LEDStates are the keys and the BlinkPatterns are the values.
 ** @brief The BlinkPatterns are the number of times to blink and the delay between blinks.
 */
LEDManager::ledStateMap_t LEDManager::ledStateMap = {
	{LEDStates_e::_LEDOff, {0, 0}},
	{LEDStates_e::_LEDOn, {0, 0}},
	{LEDStates_e::_LEDBlink, {1, 500}},
	{LEDStates_e::_LEDBlinkFast, {1, 250}},
	{LEDStates_e::_SerialManager_Start, {1, 500}},
	{LEDStates_e::_SerialManager_Stop, {1, 500}},
	{LEDStates_e::_SerialManager_Error, {1, 500}},
	{LEDStates_e::_WiFiState_None, {1, 500}},
	{LEDStates_e::_WiFiState_Connecting, {1, 500}},
	{LEDStates_e::_WiFiState_Connected, {1, 500}},
	{LEDStates_e::_WiFiState_Disconnected, {1, 500}},
	{LEDStates_e::_WiFiState_Disconnecting, {1, 500}},
	{LEDStates_e::_WiFiState_ADHOC, {1, 500}},
	{LEDStates_e::_WiFiState_Error, {1, 500}},
	{LEDStates_e::_WebServerState_None, {1, 500}},
	{LEDStates_e::_WebServerState_Starting, {1, 500}},
	{LEDStates_e::_WebServerState_Started, {1, 500}},
	{LEDStates_e::_WebServerState_Stopping, {1, 500}},
	{LEDStates_e::_WebServerState_Stopped, {1, 500}},
	{LEDStates_e::_WebServerState_Error, {1, 500}},
	{LEDStates_e::_MDNSState_None, {1, 500}},
	{LEDStates_e::_MDNSState_Starting, {1, 500}},
	{LEDStates_e::_MDNSState_Started, {1, 500}},
	{LEDStates_e::_MDNSState_Stopping, {1, 500}},
	{LEDStates_e::_MDNSState_Stopped, {1, 500}},
	{LEDStates_e::_MDNSState_Error, {1, 500}},
	{LEDStates_e::_Camera_Success, {1, 500}},
	{LEDStates_e::_Camera_Connected, {1, 500}},
	{LEDStates_e::_Camera_Disconnected, {1, 500}},
	{LEDStates_e::_Camera_Error, {1, 500}},
	{LEDStates_e::_Stream_OFF, {1, 500}},
	{LEDStates_e::_Stream_ON, {1, 500}},
	{LEDStates_e::_Stream_Error, {1, 500}},
};

//! TODO: Change the parameters for each LED state to be unique.

LEDManager::LEDManager(byte pin) : _ledPin(pin),
								   _previousMillis(0),
								   _ledState(false) {}

LEDManager::~LEDManager() {}

void LEDManager::begin()
{
	pinMode(_ledPin, OUTPUT);
	onOff(false);
}

/**
 * @brief Control the LED
 * @details This function must be called in the main loop
 *
 */
void LEDManager::handleLED(StateManager<LEDStates_e> *stateManager)
{
	if (ledStateMap.find(stateManager->getCurrentState()) != ledStateMap.end())
	{
		BlinkPatterns_t blinkPatterns = ledStateMap[stateManager->getCurrentState()]; // Get the blink pattern for the current state
		unsigned long currentMillis = millis();							// Get the current time
		if (currentMillis - _previousMillis >= blinkPatterns.delayTime) // Check if the current time is greater than the previous time plus the delay time
		{
			_previousMillis = currentMillis;
			for (int i = 0; i < blinkPatterns.times; i++)
			{
				_ledState = !_ledState;
				onOff(_ledState);
			}
		}
		stateManager->setState(LEDStates_e::_LEDOff); // Set the state to off
		onOff(false);								  // Turn the LED off
		return;
	}

	log_e("LED State not found");
	stateManager->setState(LEDStates_e::_LEDOff); // Set the state to off
	onOff(false);
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
 * @brief Blink the LED a number of times
 * @details This function is blocking and does not require the handleLED function to be called in the main loop
 * @param times number of times to blink
 * @param delayTime delay between each blink
 */
void LEDManager::blink(StateManager<LEDStates_e> *stateManager)
{

	if (ledStateMap.find(stateManager->getCurrentState()) != ledStateMap.end())
	{
		BlinkPatterns_t blinkPatterns = ledStateMap[stateManager->getCurrentState()]; // Get the blink pattern for the current state
		for (int i = 0; i < blinkPatterns.times; i++)
		{
			onOff(true);
			delay(blinkPatterns.delayTime);
			onOff(false);
			delay(blinkPatterns.delayTime);
		}
		stateManager->setState(LEDStates_e::_LEDOff); // Set the state to off
		onOff(false);								  // Turn the LED off
		return;
	}

	log_e("LED State not found");
	stateManager->setState(LEDStates_e::_LEDOff); // Set the state to off
	onOff(false);
}
