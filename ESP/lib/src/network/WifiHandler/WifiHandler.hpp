#pragma once
#ifndef WIFIHANDLER_HPP
#define WIFIHANDLER_HPP
#include <memory>
#include <string>
#include <WiFi.h>
#include "data/StateManager/StateManager.hpp"
#include "data/config/project_config.hpp"
#include "data/utilities/helpers.hpp"

class WiFiHandler
{
public:
	WiFiHandler(ProjectConfig *configManager, StateManager<WiFiState_e> *stateManager,
				std::string ssid,
				std::string password,
				uint8_t channel);
	virtual ~WiFiHandler();
	void setupWifi();

	ProjectConfig *configManager;
	StateManager<WiFiState_e> *stateManager;

	bool _enable_adhoc;

private:
	void setUpADHOC();
	void adhoc(const char *ssid, const char *password, uint8_t channel);
	void iniSTA();

	std::string ssid;
	std::string password;
	uint8_t channel;
};
#endif // WIFIHANDLER_HPP
