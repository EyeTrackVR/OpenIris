#ifndef LEDMANAGER_HPP
#define LEDMANAGER_HPP
#include <Arduino.h>
#include <data/StateManager/StateManager.hpp>
#include <unordered_map>

class LEDManager
{
public:
	LEDManager(byte pin,
			   StateManager<LEDStates_e> *stateManager);
	virtual ~LEDManager();

	void begin();
	void handleLED();
	void onOff(bool state) const;
	void blink(int times, int delayTime);

private:
	byte _ledPin;
	StateManager<LEDStates_e> *stateManager;
	unsigned long _previousMillis;
	bool _ledState;

	struct BlinkPatterns
	{
		int times;
		int delayTime;
	};

	BlinkPatterns blinkPatterns;

	typedef std::unordered_map<LEDStates_e, BlinkPatterns> ledStateMap_t;
	static ledStateMap_t ledStateMap;
};

#endif // LEDMANAGER_HPP