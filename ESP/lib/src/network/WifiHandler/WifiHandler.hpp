#pragma once
#ifndef WIFIHANDLER_HPP
#define WIFIHANDLER_HPP
#include <memory>
#include <string>
#include <vector>
#include <WiFi.h>
#include "data/StateManager/StateManager.hpp"
#include "data/config/project_config.hpp"
#include "data/utilities/helpers.hpp"

class WiFiHandler
{
public:
	WiFiHandler(ProjectConfig *configManager,
				StateManager<WiFiState_e> *stateManager,
				const std::string &ssid,
				const std::string &password,
				uint8_t channel);
	virtual ~WiFiHandler();
	void setupWifi();

	ProjectConfig *configManager;
	StateManager<WiFiState_e> *stateManager;

	bool _enable_adhoc;

private:
	void setUpADHOC();
	void adhoc(const std::string &ssid, uint8_t channel, const std::string &password = std::string());
	bool iniSTA(const std::string &ssid, const std::string &password, uint8_t channel, wifi_power_t power);

	std::string ssid;
	std::string password;
	uint8_t channel;
	uint8_t power;
};
#endif // WIFIHANDLER_HPP
