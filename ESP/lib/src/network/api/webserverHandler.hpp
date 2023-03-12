#pragma once
#ifndef XWEBSERVERHANDLER_HPP
#define XWEBSERVERHANDLER_HPP

#include "network/api/baseAPI/baseAPI.hpp"

class APIServer : public BaseAPI
{
public:
	APIServer(
			  AsyncWebServer *server,
			  ProjectConfig *projectConfig,
			  CameraHandler *camera,
			  StateManager<WiFiState_e> *wiFiStateManager,
			  const std::string &api_url);

	virtual ~APIServer();
	void setup();
	void setupServer();
	void findParam(AsyncWebServerRequest *request, const char *param, std::string &value);
	void addRouteMap(const std::string &index, route_t route, std::vector<std::string> &indexes);
	void handleRequest(AsyncWebServerRequest *request);

public:
	std::vector<std::string> indexes;
};
#endif // WEBSERVERHANDLER_HPP
