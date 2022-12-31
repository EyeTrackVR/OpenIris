#pragma once
#ifndef XWEBSERVERHANDLER_HPP
#define XWEBSERVERHANDLER_HPP

#include "network/api/baseAPI/baseAPI.hpp"

class APIServer : public BaseAPI
{
public:
	APIServer(int CONTROL_PORT,
			  ProjectConfig *projectConfig,
			  CameraHandler *camera,
			  StateManager<WiFiState_e> *WiFiStateManager,
			  const std::string &api_url);

	virtual ~APIServer();
	void begin();
	void setupServer();
	void findParam(AsyncWebServerRequest *request, const char *param, std::string &value);
	void addRouteMap(const std::string &index, route_t route, std::vector<std::string> &indexes);
	void handleRequest(AsyncWebServerRequest *request);

public:
	std::vector<std::string> indexes;
};
#endif // WEBSERVERHANDLER_HPP
