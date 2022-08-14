#include "webserverHandler.hpp"

//! These have to be called before the constructor of the class because they are static
//! C++ 11 does not have inline variables, sadly. So we have to do this.
const char *APIServer::MIMETYPE_HTML{"text/html"};
// const char *APIServer::MIMETYPE_CSS{"text/css"};
// const char *APIServer::MIMETYPE_JS{"application/javascript"};
// const char *APIServer::MIMETYPE_PNG{"image/png"};
// const char *APIServer::MIMETYPE_JPG{"image/jpeg"};
// const char *APIServer::MIMETYPE_ICO{"image/x-icon"};
const char *APIServer::MIMETYPE_JSON{"application/json"};

//*********************************************************************************************
//!                                     API Server
//*********************************************************************************************

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
  server->on("/", HTTP_GET, [&](AsyncWebServerRequest *request)
             { request->send(200); });

  // preflight cors check
  server->on("/", HTTP_OPTIONS, [&](AsyncWebServerRequest *request)
             {
        AsyncWebServerResponse* response = request->beginResponse(204);
        response->addHeader("Access-Control-Allow-Methods", "PUT,POST,GET,OPTIONS");
        response->addHeader("Access-Control-Allow-Headers", "Accept, Content-Type, Authorization, FileSize");
        response->addHeader("Access-Control-Allow-Credentials", "true");
        request->send(response); });

  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");

  // std::bind(&APIServer::API_Utilities::notFound, &api_utilities, std::placeholders::_1);
  server->onNotFound([&](AsyncWebServerRequest *request)
                     { api_utilities.notFound(request); });
  // Hex value of BUTT_PLUG_CONTROLLER == 425554545f504c55475f434f4e54524f4c4c4552
  this->server->on("/control", HTTP_GET, [&](AsyncWebServerRequest *request)
                   { command_handler(request); });

  log_d("Initializing REST API");
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
  command_map_wifi_conf.emplace("ssid", [this](const char *value) -> void
                                { setSSID(value); });
  command_map_wifi_conf.emplace("password", [this](const char *value) -> void
                                { setPass(value); });
  command_map_wifi_conf.emplace("channel", [this](const char *value) -> void
                                { setChannel(value); });

  command_map_funct.emplace("reboot_device", [this](void) -> void
                            { rebootDevice(); });
  command_map_funct.emplace("reset_config", [this](void) -> void
                            { factoryReset(); });

  command_map_json.emplace("data_json", [this](AsyncWebServerRequest *request) -> void
                           { setDataJson(request); });
  command_map_json.emplace("config_json", [this](AsyncWebServerRequest *request) -> void
                           { setConfigJson(request); });
  command_map_json.emplace("settings_json", [this](AsyncWebServerRequest *request) -> void
                           { setSettingsJson(request); });
}

void APIServer::command_handler(AsyncWebServerRequest *request)
{
  int params = request->params();
  for (int i = 0; i < params; i++)
  {
    AsyncWebParameter *param = request->getParam(i);
    {
      command_map_wifi_conf_t::const_iterator it_wifi_conf = command_map_wifi_conf.find(param->name().c_str());
      command_map_funct_t::const_iterator it_funct = command_map_funct.find(param->name().c_str());
      command_map_json_t::const_iterator it_json = command_map_json.find(param->name().c_str());

      if (it_wifi_conf != command_map_wifi_conf.end())
      {
        command_map_wifi_conf.at(param->name().c_str())(param->value().c_str());
        auto &key_it = it_wifi_conf->first;
        log_i("Command %s executed", key_it.c_str());
      }
      else if (it_funct != command_map_funct.end())
      {
        command_map_funct.at(param->name().c_str())();
        auto &key_it_funct = it_funct->first;
        log_i("Command %s executed", key_it_funct.c_str());
      }
      else if (it_json != command_map_json.end())
      {
        command_map_json.at(param->name().c_str())(request);
        auto &key_it_json = it_json->first;
        log_i("Command %s executed", key_it_json.c_str());
      }
      else
      {
        log_i("Command not found");
      }
    }
    log_i("GET[%s]: %s\n", param->name().c_str(), param->value().c_str());
  }
}

//*********************************************************************************************
//!                                     Command Functions
//*********************************************************************************************
void APIServer::setSSID(const char *value)
{
#if ENABLE_ADHOC
  network->setWiFiConf(value, network->conf->ap.ssid, &*network->conf);
#else
  network->setWiFiConf(value, network->conf->sta.ssid, &*network->conf);
#endif // ENABLE_ADHOC
}

void APIServer::setPass(const char *value)
{
#if ENABLE_ADHOC
  network->setWiFiConf(network->conf->ap.password, value, &*network->conf);
#else
  network->setWiFiConf(value, network->conf->sta.password, &*network->conf);
#endif // ENABLE_ADHOC
}

void APIServer::setChannel(const char *value)
{
#if ENABLE_ADHOC
  network->setWiFiConf(value, network->conf->ap.channel, &*network->conf);
#else
  network->setWiFiConf(value, &network->conf->sta.channel, &*network->conf);
#endif // ENABLE_ADHOC
}

void APIServer::setDataJson(AsyncWebServerRequest *request)
{
  network->configManager->getDeviceConfig()->data_json = true;
  api_utilities.my_delay(1L);
  String temp = network->configManager->getDeviceConfig()->data_json_string;
  request->send(200, MIMETYPE_JSON, temp);
  temp = "";
}

void APIServer::setConfigJson(AsyncWebServerRequest *request)
{
  network->configManager->getDeviceConfig()->config_json = true;
  api_utilities.my_delay(1L);
  String temp = network->configManager->getDeviceConfig()->config_json_string;
  request->send(200, MIMETYPE_JSON, temp);
  temp = "";
}

void APIServer::setSettingsJson(AsyncWebServerRequest *request)
{
  network->configManager->getDeviceConfig()->settings_json = true;
  api_utilities.my_delay(1L);
  String temp = network->configManager->getDeviceConfig()->settings_json_string;
  request->send(200, MIMETYPE_JSON, temp);
  temp = "";
}

void APIServer::rebootDevice()
{
  delay(20000);
  ESP.restart();
}

void APIServer::factoryReset()
{
  network->configManager->reset();
}

//*********************************************************************************************
//!                                     API Utilities
//*********************************************************************************************

APIServer::API_Utilities::API_Utilities() {}

std::string
APIServer::API_Utilities::shaEncoder(std::string data)
{
  const char *data_c = data.c_str();
  int size = 20;
  uint8_t hash[size];
  mbedtls_md_context_t ctx;
  mbedtls_md_type_t md_type = MBEDTLS_MD_SHA1;

  const size_t len = strlen(data_c);
  mbedtls_md_init(&ctx);
  mbedtls_md_setup(&ctx, mbedtls_md_info_from_type(md_type), 0);
  mbedtls_md_starts(&ctx);
  mbedtls_md_update(&ctx, (const unsigned char *)data_c, len);
  mbedtls_md_finish(&ctx, hash);
  mbedtls_md_free(&ctx);

  std::string hash_string = "";
  for (uint16_t i = 0; i < size; i++)
  {
    std::string hex = String(hash[i], HEX).c_str();
    if (hex.length() < 2)
    {
      hex = "0" + hex;
    }
    hash_string += hex;
  }
  return hash_string;
}

void APIServer::API_Utilities::notFound(AsyncWebServerRequest *request)
{
  try
  {
    log_i("%s", _networkMethodsMap[request->method()]);
  }
  catch (const std::exception &e)
  {
    log_i("UNKNOWN");
  }

  log_i(" http://%s%s/\n", request->host().c_str(), request->url().c_str());
  request->send(404, "text/plain", "Not found.");
}

void APIServer::API_Utilities::my_delay(volatile long delay_time)
{
  delay_time = delay_time * 1e6L;
  for (volatile long count = delay_time; count > 0L; count--)
    ;
}

APIServer::API_Utilities api_utilities;