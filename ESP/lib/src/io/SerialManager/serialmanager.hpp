#pragma once
#ifndef SERIAL_MANAGER_HPP
#define SERIAL_MANAGER_HPP
#include <Arduino.h>
#include <unordered_map>
#include <string>
#include <memory>
#include <serialStr.h>
#include <strTools.h>
#include <ArduinoJson.h>

#include "data/config/project_config.hpp"
#include "data/utilities/makeunique.hpp"
#include "data/utilities/helpers.hpp"

class SerialManager
{
public:
	SerialManager(ProjectConfig *projectConfig);
	virtual ~SerialManager();

	void begin();
	void handleSerial();

	friend void readStr(const char *inStr);

protected:
	ProjectConfig *projectConfig;
	std::unique_ptr<serialStr> serReader;

	enum Serial_Commands
	{
		NO_INPUT,
		DEVICE_CONFIG,
		CAMERA_CONFIG,
		WIFI_CONFIG
	};

	static std::unordered_map<std::string, Serial_Commands> command_map;
	StaticJsonDocument<1024> jsonDoc;
};

#endif // SERIAL_MANAGER_HPP