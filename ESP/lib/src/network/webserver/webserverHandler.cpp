#include "webserverHandler.hpp"

//! This has to be called before the constructor of the class because it is static
//! C++ 11 does not have inline variables, sadly. So we have to do this.
std::unordered_map<std::string, APIServer::command_func> APIServer::command_map(0);

/* Constructor with unique_ptr */
APIServer::APIServer(int CONTROL_PORT, CameraHandler *cameraHandler, WiFiHandler *network) : network(network),
                                                                                             server(new AsyncWebServer(CONTROL_PORT)),
                                                                                             cameraHandler(cameraHandler) {}

void APIServer::startAPIServer()
{
  begin();
  /* this->server->on(
      "/control",
      HTTP_GET,
      std::bind(&APIServer::command_handler, this, std::placeholders::_1)); */

  //! i have changed this to use lambdas instead of std::bind to avoid the overhead. Lambdas are always more preferable.
  this->server->on(
      "/control",
      HTTP_GET, [&](AsyncWebServerRequest *request)
      { command_handler(request); 
        request->send(200); });

  log_d("Initializing web server");
  this->server->begin();
}

void APIServer::findParam(AsyncWebServerRequest *request, const char *param, String &value)
{
  if (request->hasParam(param))
  {
    value = request->getParam(param)->value();
  }
}

void APIServer::begin()
{
  command_map.emplace("framesize", FRAME_SIZE);
  command_map.emplace("hmirror", HMIRROR);
  command_map.emplace("vflip", VFLIP);
#if ENABLE_ADHOC
  command_map.emplace("ap_ssid", AP_SSID);
  command_map.emplace("ap_password", AP_PASSWORD);
  command_map.emplace("ap_channel", AP_CHANNEL);
#else
  command_map.emplace("ssid", SSID);
  command_map.emplace("password", PASSWORD);
  command_map.emplace("channel", CHANNEL);
#endif // ENABLE_ADHOC
}

void APIServer::command_handler(AsyncWebServerRequest *request)
{
  int params = request->params();
  for (int i = 0; i < params; i++)
  {
    AsyncWebParameter *param = request->getParam(i);
    {
      switch (command_map[param->name().c_str()])
      {
      case FRAME_SIZE:
        cameraHandler->setCameraResolution((framesize_t)atoi(param->value().c_str()));
        break;
      case HMIRROR:
        cameraHandler->setHFlip(atoi(param->value().c_str()));
        break;
      case VFLIP:
        cameraHandler->setVFlip(atoi(param->value().c_str()));
        break;
#if ENABLE_ADHOC
      case AP_SSID:
        network->setWiFiConf(param->value().c_str(), network->conf->ap.ssid, &*network->conf);
        break;
      case AP_PASSWORD:
        network->setWiFiConf(param->value().c_str(), network->conf->ap.password, &*network->conf);
        break;
      case AP_CHANNEL:
        network->setWiFiConf(param->value().c_str(), &network->conf->ap.channel, &*network->conf);
        break;
#else
      case SSID:
        network->setWiFiConf(param->value().c_str(), network->conf->sta.ssid, &*network->conf);
        break;
      case PASSWORD:
        network->setWiFiConf(param->value().c_str(), network->conf->sta.password, &*network->conf);
      case CHANNEL:
        network->setWiFiConf(param->value().c_str(), &network->conf->sta.channel, &*network->conf);
        break;
#endif // ENABLE_ADHOC
      default:
        log_e("Command not found");
        break;
      }
    }
    log_i("GET[%s]: %s\n", param->name().c_str(), param->value().c_str());
  }
}