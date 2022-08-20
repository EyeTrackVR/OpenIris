#include "baseAPI.hpp"

BaseAPI::BaseAPI(int CONTROL_PORT,
				 WiFiHandler *network,
				 CameraHandler *camera,
				 StateManager<WiFiState_e> *stateManager,
				 std::string api_url) : API_Utilities(CONTROL_PORT,
													  network,
													  camera,
													  stateManager,
													  api_url) {}

BaseAPI::~BaseAPI() {}

void BaseAPI::begin()
{
	this->setupServer();
	//! i have changed this to use lambdas instead of std::bind to avoid the overhead. Lambdas are always more preferable.
	server->on("/", 0b00000001, [&](AsyncWebServerRequest *request)
			   { request->send(200); });

	// preflight cors check
	server->on("/", 0b01000000, [&](AsyncWebServerRequest *request)
			   {
        		AsyncWebServerResponse* response = request->beginResponse(204);
        		response->addHeader("Access-Control-Allow-Methods", "PUT,POST,GET,OPTIONS");
        		response->addHeader("Access-Control-Allow-Headers", "Accept, Content-Type, Authorization");
        		response->addHeader("Access-Control-Allow-Credentials", "true");
        		request->send(response); });

	DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");

	// std::bind(&BaseAPI::API_Utilities::notFound, &api_utilities, std::placeholders::_1);
	server->onNotFound([&](AsyncWebServerRequest *request)
					   { notFound(request); });
}

void BaseAPI::setupServer()
{
	localWifiConfig = {
		.ssid = "",
		.pass = "",
		.channel = 0,
		.adhoc = false,
	};

	localAPWifiConfig = {
		.ssid = "",
		.pass = "",
		.channel = 0,
		.adhoc = false,
	};
}

//*********************************************************************************************
//!                                     Command Functions
//*********************************************************************************************
void BaseAPI::setWiFi(AsyncWebServerRequest *request)
{
	switch (_networkMethodsMap_enum[request->method()])
	{
	case POST:
	{
		int params = request->params();
		for (int i = 0; i < params; i++)
		{
			AsyncWebParameter *param = request->getParam(i);
			if (network->stateManager->getCurrentState() == WiFiState_e::WiFiState_ADHOC)
			{
				localAPWifiConfig.ssid = param->value().c_str();
				localAPWifiConfig.pass = param->value().c_str();
				localAPWifiConfig.channel = atoi(param->value().c_str());
				localAPWifiConfig.adhoc = atoi(param->value().c_str());
			}
			else
			{
				localWifiConfig.ssid = param->value().c_str();
				localWifiConfig.pass = param->value().c_str();
				localWifiConfig.channel = atoi(param->value().c_str());
				localWifiConfig.adhoc = atoi(param->value().c_str());
			}
		}
		ssid_write = true;
		pass_write = true;
		channel_write = true;
		request->send(200, MIMETYPE_JSON, "{\"msg\":\"Done. Wifi Creds have been set.\"}");
		break;
	}
	default:
	{
		request->send(400, MIMETYPE_JSON, "{\"msg\":\"Invalid Request\"}");
		request->redirect("/");
		break;
	}
	}
}

/**
 * * Trigger in main loop to save config to flash
 * ? Should we force the users to update all config params before triggering a config write?
 */
void BaseAPI::triggerWifiConfigWrite()
{
	if (ssid_write && pass_write && channel_write)
	{
		ssid_write = false;
		pass_write = false;
		channel_write = false;
		if (network->stateManager->getCurrentState() == WiFiState_e::WiFiState_ADHOC)
			network->configManager->setAPWifiConfig(localAPWifiConfig.ssid.c_str(), localAPWifiConfig.pass.c_str(), &localAPWifiConfig.channel, localAPWifiConfig.adhoc, true);
		else
			network->configManager->setWifiConfig(localWifiConfig.ssid.c_str(), localWifiConfig.ssid.c_str(), localWifiConfig.pass.c_str(), &localWifiConfig.channel, localAPWifiConfig.adhoc, true);
		network->configManager->save();
	}
}

void BaseAPI::handleJson(AsyncWebServerRequest *request)
{
	std::string type = request->pathArg(0).c_str();
	switch (_networkMethodsMap_enum[request->method()])
	{
	case POST:
	{
		switch (json_TypesMap.at(type))
		{
		case DATA:
		{
			break;
		}
		case SETTINGS:
		{
			break;
		}
		case CONFIG:
		{
			break;
		}
		default:
			request->send(400, MIMETYPE_JSON, "{\"msg\":\"Invalid Request\"}");
			break;
		}
		break;
	}
	case GET:
	{
		switch (json_TypesMap.at(type))
		{
		case DATA:
		{
			network->configManager->getDeviceConfig()->data_json = true;
			Network_Utilities::my_delay(1L);
			String temp = network->configManager->getDeviceConfig()->data_json_string;
			request->send(200, MIMETYPE_JSON, temp);
			temp = "";
			break;
		}
		case SETTINGS:
		{
			network->configManager->getDeviceConfig()->config_json = true;
			Network_Utilities::my_delay(1L);
			String temp = network->configManager->getDeviceConfig()->config_json_string;
			request->send(200, MIMETYPE_JSON, temp);
			temp = "";
			break;
		}
		case CONFIG:
		{
			network->configManager->getDeviceConfig()->settings_json = true;
			Network_Utilities::my_delay(1L);
			String temp = network->configManager->getDeviceConfig()->settings_json_string;
			request->send(200, MIMETYPE_JSON, temp);
			temp = "";
			break;
		}
		default:
			request->send(400, MIMETYPE_JSON, "{\"msg\":\"Invalid Request\"}");
			break;
		}

		break;
	}
	default:
	{
		request->send(400, MIMETYPE_JSON, "{\"msg\":\"Invalid Request\"}");
		break;
	}
	}
}

void BaseAPI::rebootDevice(AsyncWebServerRequest *request)
{
	switch (_networkMethodsMap_enum[request->method()])
	{
	case GET:
	{
		delay(20000);
		request->send(200, MIMETYPE_JSON, "{\"msg\":\"Rebooting Device\"}");
		ESP.restart();
	}
	default:
	{
		request->send(400, MIMETYPE_JSON, "{\"msg\":\"Invalid Request\"}");
		break;
	}
	}
}

void BaseAPI::factoryReset(AsyncWebServerRequest *request)
{
	switch (_networkMethodsMap_enum[request->method()])
	{
	case GET:
	{
		log_d("Factory Reset");
		network->configManager->reset();
		request->send(200, MIMETYPE_JSON, "{\"msg\":\"Factory Reset\"}");
	}
	default:
	{
		request->send(400, MIMETYPE_JSON, "{\"msg\":\"Invalid Request\"}");
		break;
	}
	}
}

/**
 * @brief Remove a command handler from the API
 *
 * @param request
 * @return \c void
 */
void BaseAPI::deleteRoute(AsyncWebServerRequest *request)
{
	log_i("Request: %s", request->url().c_str());
	int params = request->params();
	auto it_map = route_map.find(request->pathArg(0).c_str());
	log_i("Request: %s", request->pathArg(0).c_str());
	if (it_map != route_map.end())
	{
		auto it = it_map->second.find(request->pathArg(1).c_str());
		if (it != it_map->second.end())
		{
			switch (_networkMethodsMap_enum[request->method()])
			{
			case DELETE:
			{
				route_map.erase(it_map->first);
				request->send(200, MIMETYPE_JSON, "{\"msg\":\"OK - Command handler removed\"}");
				break;
			}
			default:
			{
				request->send(400, MIMETYPE_JSON, "{\"msg\":\"Invalid Request\"}");
				break;
			}
			}
		}
		else
		{
			request->send(404);
		}
	}
	else
	{
		request->send(404);
	}
}

//*********************************************************************************************
//!                                     Camera Command Functions
//*********************************************************************************************

void BaseAPI::setCamera(AsyncWebServerRequest *request)
{
	switch (_networkMethodsMap_enum[request->method()])
	{
	case GET:
	{
		int params = request->params();
		for (int i = 0; i < params; i++)
		{
			AsyncWebParameter *param = request->getParam(i);
			camera->setCameraResolution((framesize_t)atoi(param->value().c_str()));
			camera->setVFlip(atoi(param->value().c_str()));
			camera->setHFlip(atoi(param->value().c_str()));
		}
		request->send(200, MIMETYPE_JSON, "{\"msg\":\"Done. Camera Settings have been set.\"}");
		break;
	}
	default:
	{
		request->send(400, MIMETYPE_JSON, "{\"msg\":\"Invalid Request\"}");
		request->redirect("/");
		break;
	}
	}
}