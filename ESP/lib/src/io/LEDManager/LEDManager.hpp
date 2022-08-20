#ifndef LEDMANAGER_HPP
#define LEDMANAGER_HPP
#include <Arduino.h>

class LEDManager
{
public:
	LEDManager(byte pin);
	virtual ~LEDManager();

	void begin();
	void onOff(bool state) const;
	void blink(unsigned long time);
	void displayStatus();

private:
	byte _ledPin;
	unsigned long _previousMillis;
	bool _ledState;
};

#endif // LEDMANAGER_HPP