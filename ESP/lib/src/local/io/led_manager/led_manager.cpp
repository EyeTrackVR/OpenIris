#include "led_manager.hpp"

/**
 *! @brief This is declared as a static member of the class - therefor it must
 *be initialized outside of the class.
 ** @brief This is a map of the LEDStates and the BlinkPatterns.
 ** @brief The LEDStates are the keys and the BlinkPatterns are the values.
 ** @brief The BlinkPatterns are the number of times to blink and the delay
 *between blinks.
 */

LEDManager::ledStateMap_t LEDManager::ledStateMap = {
    {LEDStates_e::_LedStateNone, {{0, 500}}},
    {LEDStates_e::_Improv_Error,
     {{1, 1000}, {0, 500}, {0, 1000}, {0, 500}, {1, 1000}}},
    {LEDStates_e::_Improv_Start,
     {{1, 500}, {0, 300}, {0, 300}, {0, 300}, {1, 500}}},
    {LEDStates_e::_Improv_Stop,
     {{1, 300}, {0, 500}, {0, 500}, {0, 500}, {1, 300}}},
    {LEDStates_e::_Improv_Processing,
     {{1, 200}, {0, 100}, {0, 500}, {0, 100}, {1, 200}}},
    {LEDStates_e::_WebServerState_Error,
     {{1, 200}, {0, 100}, {0, 500}, {0, 100}, {1, 200}}},
    {LEDStates_e::_WiFiState_Error,
     {{1, 200}, {0, 100}, {0, 500}, {0, 100}, {1, 200}}},
    {LEDStates_e::_MDNSState_Error,
     {{1, 200},
      {0, 100},
      {1, 200},
      {0, 100},
      {0, 500},
      {0, 100},
      {1, 200},
      {0, 100},
      {1, 200}}},
    {LEDStates_e::_Camera_Error,
     {{1, 5000}}},  // this also works as a more general error - something went
                    // critically wrong? We go here
    {LEDStates_e::_WiFiState_Connecting, {{1, 100}, {0, 100}}},
    {LEDStates_e::_WiFiState_Connected,
     {{1, 100},
      {0, 100},
      {1, 100},
      {0, 100},
      {1, 100},
      {0, 100},
      {1, 100},
      {0, 100},
      {1, 100},
      {0, 100}}}};

std::vector<LEDStates_e> LEDManager::keepAliveStates = {
    LEDStates_e::_WebServerState_Error, LEDStates_e::_Camera_Error};

LEDManager::LEDManager(byte pin) : _ledPin(pin), _ledState(false) {}

LEDManager::~LEDManager() {}

void LEDManager::begin() {
  pinMode(_ledPin, OUTPUT);
  // the defualt state is _LedStateNone so we're fine
  this->currentState = ledStateManager.getCurrentState();
  this->currentPatternIndex = 0;
  BlinkPatterns_t pattern =
      this->ledStateMap[this->currentState][this->currentPatternIndex];
  this->toggleLED(pattern.state);
  this->nextStateChangeMillis = pattern.delayTime;

  log_d("begin %d", this->currentPatternIndex);
}

/**
 * @brief Display the current state of the LED manager as a pattern of blinking
 * LED
 * @details This function must be called in the main loop
 */
void LEDManager::handleLED() {
  if (millis() <= this->nextStateChangeMillis) {
    return;
  }

  // !TODO what if we want a looping state? Or a state that needs to stay
  // bright? Am overthinking this, aren't I?

  // we've reached the timeout on that state, check if we can grab next one and
  // start displaying it, or if we need to keep displaying the current one
  if (this->currentPatternIndex >
      this->ledStateMap[this->currentState].size() - 1) {
    auto nextState = ledStateManager.getCurrentState();
    // we want to keep displaying the same state only if its an keepAlive one,
    // but we should change if the incoming one is also an errours state, maybe
    // more serious one this time <- this may be a bad idea
    if ((std::find(this->keepAliveStates.begin(), this->keepAliveStates.end(),
                   this->currentState) != this->keepAliveStates.end() ||
         std::find(this->keepAliveStates.begin(), this->keepAliveStates.end(),
                   nextState) != this->keepAliveStates.end()) ||
        (this->currentState != nextState &&
         this->ledStateMap.find(nextState) != this->ledStateMap.end())) {
      log_d("Updating the state and reseting");
      this->toggleLED(false);
      this->currentState = nextState;
      this->currentPatternIndex = 0;
      BlinkPatterns_t pattern =
          this->ledStateMap[this->currentState][this->currentPatternIndex];
      this->nextStateChangeMillis = millis() + pattern.delayTime;
      return;
    }
    // it wasn't a keepAlive state, nor did we have another one ready,
    // we're done for now
    this->toggleLED(false);
    return;
  }
  // we can safely advance it and display the next stage
  BlinkPatterns_t pattern =
      this->ledStateMap[this->currentState][this->currentPatternIndex];
  this->toggleLED(pattern.state);
  this->nextStateChangeMillis = millis() + pattern.delayTime;
  log_d("before updating stage %d", this->currentPatternIndex);
  this->currentPatternIndex++;
  log_d("updated stage %d", this->currentPatternIndex);
}

/**
 * @brief Turn the LED on or off
 *
 * @param state
 */
void LEDManager::toggleLED(bool state) const {
  digitalWrite(_ledPin, state);
}
