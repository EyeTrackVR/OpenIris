#ifndef LEDMANAGER_HPP
#define LEDMANAGER_HPP
#include <vector>
#include <Arduino.h>
#include <data/StateManager/StateManager.hpp>
#include <unordered_map>

class LEDManager
{
public:
	LEDManager(byte pin, StateManager<LEDStates_e> *stateManager);
	virtual ~LEDManager();

	void begin();
	void handleLED();
	void toggleLED(bool state) const;

private:
	byte _ledPin;
	StateManager<LEDStates_e> *_stateManager;
	unsigned long nextStateChangeMillis = 0;
	bool _ledState;

	struct BlinkPatterns_t
	{
		int state;
		int delayTime;
	};

	typedef std::unordered_map<LEDStates_e, std::vector<BlinkPatterns_t>> ledStateMap_t;
	static ledStateMap_t ledStateMap;
	static std::vector<LEDStates_e> keepAliveStates;
	LEDStates_e currentState;
	unsigned int currentPatternIndex = 0;
};

#endif // LEDMANAGER_HPP