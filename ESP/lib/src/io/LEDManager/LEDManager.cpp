#include "LEDManager.hpp"

/**
 *! @brief This is declared as a static member of the class - therefor it must be initialized outside of the class.
 ** @brief This is a map of the LEDStates and the BlinkPatterns.
 ** @brief The LEDStates are the keys and the BlinkPatterns are the values.
 ** @brief The BlinkPatterns are the number of times to blink and the delay between blinks.
 */

// side note - do we need a separate state for the led maanger?
// we could get away with using pubsub pattern 
// like, register led manager to anything that needs its state displayed
// and we just update the current held pattern list 
LEDManager::ledStateMap_t LEDManager::ledStateMap = {
	{LEDStates_e::_SerialManager_Error, {{1, 200}, {0, 100}, {0, 500}, {0, 100}, {1, 200}}}, 
	{LEDStates_e::_WebServerState_Error, {{1, 200}, {0, 100}, {0, 500}, {0, 100}, {1, 200}}},
	{LEDStates_e::_WiFiState_Error, {{1, 200}, {0, 100}, {0, 500}, {0, 100}, {1, 200}}},
	{LEDStates_e::_MDNSState_Error, {{1, 200}, {0, 100}, {1, 200}, {0, 100}, {0, 500}, {0, 100}, {1, 200}, {0, 100}, {1, 200}}},
	{LEDStates_e::_Camera_Error, {{1, 500}}}, // this also works as a more general error - something went critically wrong? We go here
	{LEDStates_e::_WiFiState_Connecting, {{1, 100}, {0, 100}}},
	{LEDStates_e::_WiFiState_Connected, {{1, 50}, {0, 50}, {1, 50}, {0, 50}, {1, 50}, {0, 50}, {1, 50}, {0, 50}, {1, 50}, {0, 50}}}
};

LEDManager::LEDManager(
	byte pin,
	 StateManager<LEDStates_e> *stateManager) : _ledPin(pin),
												_stateManager(stateManager),
												_ledState(false) {}

LEDManager::~LEDManager() {}

void LEDManager::begin()
{
	pinMode(_ledPin, OUTPUT);
	this->toggleLED(false);
	this->currentState = this->_stateManager->getCurrentState(); // no idea if we should be checking for empty state here
	BlinkPatterns_t pattern = this->ledStateMap[this->currentState][this->currentPatternIndex];
	this->nextStateChangeMillis = pattern.delayTime;
}

/**
 * @brief Display the current state of the LED manager as a pattern of blinking LED
 * @details This function must be called in the main loop
 */
void LEDManager::handleLED() {
	if(millis() >= this->nextStateChangeMillis){
		// we've waited enough, let's check if we're finished with this state
		// and if so, grab the next one if there is one, otherwise we're finished
		if (this->currentPatternIndex > this->ledStateMap[this->currentState].size() - 1){
			auto nextState = this->_stateManager->getCurrentState();
			if(this->ledStateMap.find(nextState) != this->ledStateMap.end()){
				this->toggleLED(false);
				this->currentState = nextState;
				this->currentPatternIndex = 0; // because we will be advancing it as the next step
				BlinkPatterns_t pattern = this->ledStateMap[this->currentState][this->currentPatternIndex];
				this->nextStateChangeMillis = millis() + pattern.delayTime;
			} else {
				return;
			}
		}
		// we can, so let's grab it and advance the timer
		BlinkPatterns_t pattern = this->ledStateMap[this->currentState][this->currentPatternIndex];
		log_d("state, currentTime, stateChangeMillis %d %d %d", pattern.state, this->currentPatternIndex, this->nextStateChangeMillis );
		this->toggleLED(pattern.state);
		this->nextStateChangeMillis = millis() + pattern.delayTime;
		// we've passed the time of this state being on, let's advance it
		this->currentPatternIndex += 1;
	}
}

/**
 * @brief Turn the LED on or off
 *
 * @param state
 */
void LEDManager::toggleLED(bool state) const
{
	digitalWrite(_ledPin, state);
}
