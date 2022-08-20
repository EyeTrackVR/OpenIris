#include "webserverHandler.hpp"

//*********************************************************************************************
//!                                     API Server
//*********************************************************************************************

APIServer::APIServer(int CONTROL_PORT,
					 WiFiHandler *network,
					 CameraHandler *camera,
					 StateManager<WiFiState_e> *stateManager,
					 std::string api_url) : BaseAPI(CONTROL_PORT,
													network,
													camera,
													stateManager,
													api_url) {}

APIServer::~APIServer() {}

void APIServer::begin()
{
	log_d("Initializing REST API");
	this->setupServer();
	BaseAPI::begin();

	char buffer[1000];
	snprintf(buffer, sizeof(buffer), "^\\%s\\/([a-zA-Z0-9]+)\\/command\\/([a-zA-Z0-9]+)$", this->api_url.c_str());
	log_d("API URL: %s", buffer);
	server->on(buffer, 0b01111111, [&](AsyncWebServerRequest *request)
			   { handleRequest(request); });

	server->begin();
}

void APIServer::setupServer()
{
	// Set case NULL_METHOD routes
	routes.emplace("wifi", &APIServer::setWiFi);
	routes.emplace("reset_config", &APIServer::factoryReset);
	routes.emplace("reboot_device", &APIServer::rebootDevice);
	routes.emplace("set_json", &APIServer::handleJson);
	routes.emplace("set_camera", &APIServer::setCamera);
	routes.emplace("delete_route", &APIServer::deleteRoute);

	routeHandler("builtin", routes); // add new map to the route map
}

void APIServer::findParam(AsyncWebServerRequest *request, const char *param, String &value)
{
	if (request->hasParam(param))
	{
		value = request->getParam(param)->value();
	}
}

/**
 * @brief Add a command handler to the API
 *
 * @param index
 * @param funct
 * @return \c vector<string> a list of the indexes of the command handlers
 */
std::vector<std::string> APIServer::routeHandler(std::string index, route_t route)
{
	route_map.emplace(index, route);
	std::vector<std::string> indexes;
	indexes.reserve(route.size());

	for (const auto &key : route)
	{
		indexes.push_back(key.first);
	}

	return indexes;
}

void APIServer::handleRequest(AsyncWebServerRequest *request)
{
	// Get the route
	log_i("Request: %s", request->url().c_str());
	int params = request->params();
	auto it_map = route_map.find(request->pathArg(0).c_str());
	log_i("Request: %s", request->pathArg(0).c_str());
	auto it_method = it_map->second.find(request->pathArg(1).c_str());
	log_i("Request: %s", request->pathArg(1).c_str());

	for (int i = 0; i < params; i++)
	{
		AsyncWebParameter *param = request->getParam(i);
		{
			{
				if (it_map != route_map.end())
				{
					if (it_method != it_map->second.end())
					{
						(*this.*(it_method->second))(request);
					}
					else
					{
						request->send(400, MIMETYPE_JSON, "{\"msg\":\"Invalid Command\"}");
						request->redirect("/");
						return;
					}
				}
				else
				{
					request->send(400, MIMETYPE_JSON, "{\"msg\":\"Invalid Map Index\"}");
					request->redirect("/");
					return;
				}
			}
			log_i("%s[%s]: %s\n", _networkMethodsMap[request->method()].c_str(), param->name().c_str(), param->value().c_str());
		}
	}
	request->send(200, MIMETYPE_JSON, "{\"msg\":\"Command executed\"}");
}
