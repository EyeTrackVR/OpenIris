#ifndef LEDMANAGER_HPP
#define LEDMANAGER_HPP
#include <Arduino.h>
#include <local/data/statemanager/state_manager.hpp>
#include <unordered_map>
#include <vector>

class LEDManager {
 public:
  LEDManager(byte pin);
  virtual ~LEDManager();

  void begin();
  void handleLED();
  void toggleLED(bool state) const;

 private:
  byte _ledPin;
  unsigned long nextStateChangeMillis = 0;
  bool _ledState;

  struct BlinkPatterns_t {
    int state;
    int delayTime;
  };

  typedef std::unordered_map<LEDStates_e, std::vector<BlinkPatterns_t>>
      ledStateMap_t;
  static ledStateMap_t ledStateMap;
  static std::vector<LEDStates_e> keepAliveStates;
  LEDStates_e currentState;
  unsigned int currentPatternIndex = 0;
};

#endif  // LEDMANAGER_HPP
