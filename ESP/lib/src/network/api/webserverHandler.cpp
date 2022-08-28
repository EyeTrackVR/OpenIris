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
	routes.emplace("wifi", &APIServer::setWiFi);
	routes.emplace("resetConfig", &APIServer::factoryReset);
	routes.emplace("rebootDevice", &APIServer::rebootDevice);
	routes.emplace("setJson", &APIServer::handleJson);
	routes.emplace("setCamera", &APIServer::setCamera);
	routes.emplace("deleteRoute", &APIServer::deleteRoute);

	// Camera Routes
	

	//! reserve enough memory for all routes - must be called after adding routes and before adding routes to route_map
	indexes.reserve(routes.size());			 // this is done to avoid reallocation of memory and copying of data
	addRouteMap("builtin", routes, indexes); // add new route map to the route_map
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
 * @param indexes \c std::vector<std::string> a list of the routes of the command handlers
 *
 * @return void
 *
 */
void APIServer::addRouteMap(std::string index, route_t route, std::vector<std::string> &indexes)
{
	route_map.emplace(index, route);

	for (const auto &key : route)
	{
		indexes.emplace_back(key.first); // add the route to the list of routes - use emplace_back to avoid copying
	}
}

void APIServer::handleRequest(AsyncWebServerRequest *request)
{
	try
	{
		// Get the route
		log_i("Request URL: %s", request->url().c_str());
		int params = request->params();
		auto it_map = route_map.find(request->pathArg(0).c_str());
		log_i("Request First Arg: %s", request->pathArg(0).c_str());
		auto it_method = it_map->second.find(request->pathArg(1).c_str());
		log_i("Request Second Arg: %s", request->pathArg(1).c_str());

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
	catch (...) // catch all exceptions
	{
		request->send(400, MIMETYPE_JSON, "{\"msg\":\"An Error has occurred\"}");
	}
}
