#ifndef LEDMANAGER_HPP
#define LEDMANAGER_HPP
#include <Arduino.h>
#include <data/StateManager/StateManager.hpp>
#include <unordered_map>

class LEDManager
{
public:
	LEDManager(byte pin);
	virtual ~LEDManager();

	void begin();
	void handleLED(StateManager<LEDStates_e> *stateManager);
	void onOff(bool state) const;
	void blink(StateManager<LEDStates_e> *stateManager);

private:
	byte _ledPin;
	unsigned long _previousMillis;
	bool _ledState;

	struct BlinkPatterns_t
	{
		int times;
		int delayTime;
	};

	typedef std::unordered_map<LEDStates_e, BlinkPatterns_t> ledStateMap_t;
	static ledStateMap_t ledStateMap;
};

#endif // LEDMANAGER_HPP