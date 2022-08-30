#include "baseAPI.hpp"

BaseAPI::BaseAPI(int CONTROL_PORT,
				 WiFiHandler *network,
				 CameraHandler *camera,
				 StateManager<WiFiState_e> *stateManager,
				 const std::string &api_url) : API_Utilities(CONTROL_PORT,
															 network,
															 camera,
															 stateManager,
															 api_url) {}

BaseAPI::~BaseAPI() {}

void BaseAPI::begin()
{
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

//*********************************************************************************************
//!                                     Command Functions
//*********************************************************************************************
void BaseAPI::setWiFi(AsyncWebServerRequest *request)
{
	switch (_networkMethodsMap_enum[request->method()])
	{
	case POST:
	{
		size_t params = request->params();

		std::string ssid = std::string();
		std::string password = std::string();
		int channel = 0;
		bool adhoc = false;

		log_d("Number of Params: %d", params);
		for (size_t i = 0; i < params; i++)
		{
			AsyncWebParameter *param = request->getParam(i);
			if (param->name() == "ssid")
			{
				ssid = param->value().c_str();
			}
			else if (param->name() == "password")
			{
				password = param->value().c_str();
			}
			else if (param->name() == "channel")
			{
				channel = atoi(param->value().c_str());
			}
			else if (param->name() == "adhoc")
			{
				adhoc = atoi(param->value().c_str());
			}
			log_i("%s[%s]: %s\n", _networkMethodsMap[request->method()].c_str(), param->name().c_str(), param->value().c_str());
		}

		if (network->stateManager->getCurrentState() == WiFiState_e::WiFiState_ADHOC)
		{
			network->configManager->setAPWifiConfig(ssid, password, (uint8_t *)channel, adhoc, true);
		}
		else
		{
			network->configManager->setWifiConfig(ssid, ssid, password, (uint8_t *)channel, adhoc, true);
		}

		request->send(200, MIMETYPE_JSON, "{\"msg\":\"Done. Wifi Creds have been set.\"}");
		network->configManager->wifiConfigSave();
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

//! TODO: ADD JSON handlers for the POST requests
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
			std::string temp = network->configManager->getDeviceConfig()->data_json_string;
			request->send(200, MIMETYPE_JSON, temp.c_str());
			temp = std::string();
			break;
		}
		case SETTINGS:
		{
			network->configManager->getDeviceConfig()->config_json = true;
			Network_Utilities::my_delay(1L);
			std::string temp = network->configManager->getDeviceConfig()->config_json_string;
			request->send(200, MIMETYPE_JSON, temp.c_str());
			temp = std::string();
			break;
		}
		case CONFIG:
		{
			network->configManager->getDeviceConfig()->settings_json = true;
			Network_Utilities::my_delay(1L);
			std::string temp = network->configManager->getDeviceConfig()->settings_json_string;
			request->send(200, MIMETYPE_JSON, temp.c_str());
			temp = std::string();
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
		// create temporary variables to store the values
		int temp_camera_framesize = 0;
		int temp_camera_vflip = 0;
		int temp_camera_hflip = 0;
		int temp_camera_quality = 0;

		int params = request->params();
		for (int i = 0; i < params; i++)
		{
			AsyncWebParameter *param = request->getParam(i);
			if (param->name() == "framesize")
			{
				temp_camera_framesize = param->value().toInt();
			}
			else if (param->name() == "vflip")
			{
				temp_camera_vflip = param->value().toInt();
			}
			else if (param->name() == "hflip")
			{
				temp_camera_hflip = param->value().toInt();
			}
			else if (param->name() == "quality")
			{
				temp_camera_quality = param->value().toInt();
			}
		}

		// set the values for this instance
		camera->setCameraResolution((framesize_t)temp_camera_framesize);
		camera->setVFlip(temp_camera_vflip);
		camera->setHFlip(temp_camera_hflip);
		//! TODO: Need to add -> camera->setQuality(temp_camera_quality);

		network->configManager->setCameraConfig((uint8_t *)temp_camera_vflip, (uint8_t *)temp_camera_framesize, (uint8_t *)temp_camera_hflip, (uint8_t *)temp_camera_quality, true);
		network->configManager->cameraConfigSave();



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

//! TODO: Optimize this!!
void BaseAPI::getCameraStatus(AsyncWebServerRequest *request)
{
	static char json_response[1024];

	sensor_t *s = esp_camera_sensor_get();
	if (s == NULL)
	{
		request->send(501);
		return;
	}
	char *p = json_response;
	*p++ = '{';

	p += sprintf(p, "\"framesize\":%u,", s->status.framesize);
	p += sprintf(p, "\"quality\":%u,", s->status.quality);
	p += sprintf(p, "\"brightness\":%d,", s->status.brightness);
	p += sprintf(p, "\"contrast\":%d,", s->status.contrast);
	p += sprintf(p, "\"saturation\":%d,", s->status.saturation);
	p += sprintf(p, "\"sharpness\":%d,", s->status.sharpness);
	p += sprintf(p, "\"special_effect\":%u,", s->status.special_effect);
	p += sprintf(p, "\"wb_mode\":%u,", s->status.wb_mode);
	p += sprintf(p, "\"awb\":%u,", s->status.awb);
	p += sprintf(p, "\"awb_gain\":%u,", s->status.awb_gain);
	p += sprintf(p, "\"aec\":%u,", s->status.aec);
	p += sprintf(p, "\"aec2\":%u,", s->status.aec2);
	p += sprintf(p, "\"denoise\":%u,", s->status.denoise);
	p += sprintf(p, "\"ae_level\":%d,", s->status.ae_level);
	p += sprintf(p, "\"aec_value\":%u,", s->status.aec_value);
	p += sprintf(p, "\"agc\":%u,", s->status.agc);
	p += sprintf(p, "\"agc_gain\":%u,", s->status.agc_gain);
	p += sprintf(p, "\"gainceiling\":%u,", s->status.gainceiling);
	p += sprintf(p, "\"bpc\":%u,", s->status.bpc);
	p += sprintf(p, "\"wpc\":%u,", s->status.wpc);
	p += sprintf(p, "\"raw_gma\":%u,", s->status.raw_gma);
	p += sprintf(p, "\"lenc\":%u,", s->status.lenc);
	p += sprintf(p, "\"hmirror\":%u,", s->status.hmirror);
	p += sprintf(p, "\"vflip\":%u,", s->status.vflip);
	p += sprintf(p, "\"dcw\":%u,", s->status.dcw);
	p += sprintf(p, "\"colorbar\":%u", s->status.colorbar);
	*p++ = '}';
	*p++ = 0;

	AsyncWebServerResponse *response = request->beginResponse(200, MIMETYPE_JSON, json_response);
	response->addHeader("Access-Control-Allow-Origin", "*");
	request->send(response);
}

//! TODO: Optimize this!!
//! Change this to a hashmap and a switch-case to remove the string comparisons and if statements
void BaseAPI::setCameraVar(AsyncWebServerRequest *request)
{
	if (!request->hasArg("var") || !request->hasArg("val"))
	{
		request->send(404);
		return;
	}
	String var = request->arg("var");
	const char *variable = var.c_str();
	int val = atoi(request->arg("val").c_str());

	sensor_t *s = esp_camera_sensor_get();
	if (s == NULL)
	{
		request->send(501);
		return;
	}

	int res = 0;
	if (!strcmp(variable, "framesize"))
		res = s->set_framesize(s, (framesize_t)val);
	else if (!strcmp(variable, "quality"))
		res = s->set_quality(s, val);
	else if (!strcmp(variable, "contrast"))
		res = s->set_contrast(s, val);
	else if (!strcmp(variable, "brightness"))
		res = s->set_brightness(s, val);
	else if (!strcmp(variable, "saturation"))
		res = s->set_saturation(s, val);
	else if (!strcmp(variable, "sharpness"))
		res = s->set_sharpness(s, val);
	else if (!strcmp(variable, "gainceiling"))
		res = s->set_gainceiling(s, (gainceiling_t)val);
	else if (!strcmp(variable, "colorbar"))
		res = s->set_colorbar(s, val);
	else if (!strcmp(variable, "awb"))
		res = s->set_whitebal(s, val);
	else if (!strcmp(variable, "agc"))
		res = s->set_gain_ctrl(s, val);
	else if (!strcmp(variable, "aec"))
		res = s->set_exposure_ctrl(s, val);
	else if (!strcmp(variable, "hmirror"))
		res = s->set_hmirror(s, val);
	else if (!strcmp(variable, "vflip"))
		res = s->set_vflip(s, val);
	else if (!strcmp(variable, "awb_gain"))
		res = s->set_awb_gain(s, val);
	else if (!strcmp(variable, "agc_gain"))
		res = s->set_agc_gain(s, val);
	else if (!strcmp(variable, "aec_value"))
		res = s->set_aec_value(s, val);
	else if (!strcmp(variable, "aec2"))
		res = s->set_aec2(s, val);
	else if (!strcmp(variable, "denoise"))
		res = s->set_denoise(s, val);
	else if (!strcmp(variable, "dcw"))
		res = s->set_dcw(s, val);
	else if (!strcmp(variable, "bpc"))
		res = s->set_bpc(s, val);
	else if (!strcmp(variable, "wpc"))
		res = s->set_wpc(s, val);
	else if (!strcmp(variable, "raw_gma"))
		res = s->set_raw_gma(s, val);
	else if (!strcmp(variable, "lenc"))
		res = s->set_lenc(s, val);
	else if (!strcmp(variable, "special_effect"))
		res = s->set_special_effect(s, val);
	else if (!strcmp(variable, "wb_mode"))
		res = s->set_wb_mode(s, val);
	else if (!strcmp(variable, "ae_level"))
		res = s->set_ae_level(s, val);

	else
	{
		log_e("unknown setting %s", var.c_str());
		request->send(404);
		return;
	}
	log_d("Got setting %s with value %d. Res: %d", var.c_str(), val, res);

	AsyncWebServerResponse *response = request->beginResponse(200);
	response->addHeader("Access-Control-Allow-Origin", "*");
	request->send(response);
}
