#include "baseAPI.hpp"

//! These have to be called before the constructor of the class because they are
//! static C++ 11 does not have inline variables, sadly. So we have to do this.
// const char *BaseAPI::MIMETYPE_HTML{"text/html"};
// const char *BaseAPI::MIMETYPE_CSS{"text/css"};
// const char *BaseAPI::MIMETYPE_JS{"application/javascript"};
// const char *BaseAPI::MIMETYPE_PNG{"image/png"};
// const char *BaseAPI::MIMETYPE_JPG{"image/jpeg"};
// const char *BaseAPI::MIMETYPE_ICO{"image/x-icon"};
const char* BaseAPI::MIMETYPE_JSON{"application/json"};

BaseAPI::BaseAPI(int CONTROL_PORT,
                 ProjectConfig* projectConfig,
                 CameraHandler* camera,
                 StateManager<WiFiState_e>* WiFiStateManager,
                 const std::string& api_url)
    : server(CONTROL_PORT),
      projectConfig(projectConfig),
      camera(camera),
      wiFiStateManager(wiFiStateManager),
      api_url(api_url) {}

BaseAPI::~BaseAPI() {}

void BaseAPI::begin() {
  //! i have changed this to use lambdas instead of std::bind to avoid the
  //! overhead. Lambdas are always more preferable.
  server.on("/", 0b00000001,
            [&](AsyncWebServerRequest* request) { request->send(200); });

  // preflight cors check
  server.on("/", 0b01000000, [&](AsyncWebServerRequest* request) {
    AsyncWebServerResponse* response = request->beginResponse(204);
    response->addHeader("Access-Control-Allow-Methods", "PUT,POST,GET,OPTIONS");
    response->addHeader("Access-Control-Allow-Headers",
                        "Accept, Content-Type, Authorization");
    response->addHeader("Access-Control-Allow-Credentials", "true");
    request->send(response);
  });

  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");

  // std::bind(&BaseAPI::notFound, &std::placeholders::_1);
  server.onNotFound([&](AsyncWebServerRequest* request) { notFound(request); });
}

void BaseAPI::notFound(AsyncWebServerRequest* request) const {
  if (_networkMethodsMap.find(request->method()) != _networkMethodsMap.end()) {
    log_i("%s: http://%s%s/\n",
          _networkMethodsMap.at(request->method()).c_str(),
          request->host().c_str(), request->url().c_str());
    char buffer[100];
    snprintf(buffer, sizeof(buffer), "Request %s Not found: %s",
             _networkMethodsMap.at(request->method()).c_str(),
             request->url().c_str());
    request->send(404, "text/plain", buffer);
  } else {
    request->send(404, "text/plain", "Request Not found using unknown method");
  }
}

//*********************************************************************************************
//!                                     Command Functions
//*********************************************************************************************
void BaseAPI::setWiFi(AsyncWebServerRequest* request) {
  switch (_networkMethodsMap_enum[request->method()]) {
    case POST: {
      int params = request->params();
      std::string networkName;
      std::string ssid;
      std::string password;
      uint8_t channel = 0;
      uint8_t power = 0;
      uint8_t adhoc = 0;

      log_d("Number of Params: %d", params);
      for (int i = 0; i < params; i++) {
        AsyncWebParameter* param = request->getParam(i);
        if (param->name() == "networkName") {
          networkName = param->value().c_str();
        } else if (param->name() == "ssid") {
          ssid = param->value().c_str();
        } else if (param->name() == "password") {
          password = param->value().c_str();
        } else if (param->name() == "channel") {
          channel = (uint8_t)atoi(param->value().c_str());
        } else if (param->name() == "power") {
          power = (uint8_t)atoi(param->value().c_str());
        } else if (param->name() == "adhoc") {
          adhoc = (uint8_t)atoi(param->value().c_str());
        }

        log_i("%s[%s]: %s\n", _networkMethodsMap[request->method()].c_str(),
              param->name().c_str(), param->value().c_str());
      }
      // note: We're passing empty params by design, this is done to reset
      // specific fields
      projectConfig->setWifiConfig(networkName, ssid, password, &channel,
                                   &power, adhoc, true);

      /* if (WiFiStateManager->getCurrentState() ==
      WiFiState_e::WiFiState_ADHOC)
      {
              projectConfig->setAPWifiConfig(ssid, password, &channel, adhoc,
      true);
      }
      else
      {

      } */

      request->send(200, MIMETYPE_JSON,
                    "{\"msg\":\"Done. Wifi Creds have been set.\"}");
      break;
    }
    case DELETE: {
      projectConfig->deleteWifiConfig(request->arg("networkName").c_str(),
                                      true);
      request->send(200, MIMETYPE_JSON,
                    "{\"msg\":\"Done. Wifi Creds have been deleted.\"}");
      break;
    }
    default: {
      request->send(400, MIMETYPE_JSON, "{\"msg\":\"Invalid Request\"}");
      request->redirect("/");
      break;
    }
  }
}

void BaseAPI::getJsonConfig(AsyncWebServerRequest* request) {
  // returns the current stored config in case it get's deleted on the PC.
  switch (_networkMethodsMap_enum[request->method()]) {
    case GET: {
      std::string wifiConfigSerialized = "\"wifi_config\": [";
      auto networksConfigs = projectConfig->getWifiConfigs();
      for (auto networkIterator = networksConfigs->begin();
           networkIterator != networksConfigs->end(); networkIterator++) {
        wifiConfigSerialized +=
            networkIterator->toRepresentation() +
            (std::next(networkIterator) != networksConfigs->end() ? "," : "");
      }
      wifiConfigSerialized += "]";

      std::string json = Helpers::format_string(
          "{%s, %s, %s, %s, %s}",
          projectConfig->getDeviceConfig()->toRepresentation().c_str(),
          projectConfig->getCameraConfig()->toRepresentation().c_str(),
          wifiConfigSerialized.c_str(),
          projectConfig->getMDNSConfig()->toRepresentation().c_str(),
          projectConfig->getAPWifiConfig()->toRepresentation().c_str());
      request->send(200, MIMETYPE_JSON, json.c_str());
      break;
    }
    default: {
      request->send(400, MIMETYPE_JSON, "{\"msg\":\"Invalid Request\"}");
      break;
    }
  }
}

void BaseAPI::setDeviceConfig(AsyncWebServerRequest* request) {
  switch (_networkMethodsMap_enum[request->method()]) {
    case GET: {
      request->send(400, MIMETYPE_JSON, "{\"msg\":\"Invalid Request\"}");
      break;
    }
    case POST: {
      int params = request->params();

      std::string hostname;
      std::string service;
      std::string ota_password;
      std::string firmware_name;
      int ota_port;

      for (int i = 0; i < params; i++) {
        AsyncWebParameter* param = request->getParam(i);
        if (param->name() == "hostname") {
          hostname.assign(param->value().c_str());
        } else if (param->name() == "service") {
          service.assign(param->value().c_str());
        } else if (param->name() == "ota_port") {
          ota_port = atoi(param->value().c_str());
        } else if (param->name() == "ota_password") {
          ota_password.assign(param->value().c_str());
        } else if (param->name() == "firmware_name") {
          firmware_name.assign(param->value().c_str());
        }
      }
      // note: We're passing empty params by design, this is done to reset
      // specific fields
      projectConfig->setDeviceConfig(ota_password, ota_port, firmware_name,
                                     true);
      projectConfig->setMDNSConfig(hostname, service, true);
      request->send(200, MIMETYPE_JSON,
                    "{\"msg\":\"Done. Device Config has been set.\"}");
    }
  }
}

void BaseAPI::setWiFiTXPower(AsyncWebServerRequest* request) {
  switch (_networkMethodsMap_enum[request->method()]) {
    case GET: {
      int params = request->params();

      uint8_t txPower = 0;

      for (int i = 0; i < params; i++) {
        AsyncWebParameter* param = request->getParam(i);
        if (param->name() == "txPower") {
          txPower = atoi(param->value().c_str());
        }
      }
      projectConfig->setWiFiTxPower(&txPower, true);
      projectConfig->wifiTxPowerConfigSave();
      request->send(200, MIMETYPE_JSON,
                    "{\"msg\":\"Done. TX Power has been set.\"}");
      break;
    }
    case POST: {
      int params = request->params();

      uint8_t txPower = 0;

      for (int i = 0; i < params; i++) {
        AsyncWebParameter* param = request->getParam(i);
        if (param->name() == "txPower") {
          txPower = atoi(param->value().c_str());
        }
      }
      projectConfig->setWiFiTxPower(&txPower, true);
      projectConfig->wifiTxPowerConfigSave();
      request->send(200, MIMETYPE_JSON,
                    "{\"msg\":\"Done. TX Power has been set.\"}");
    }
  }
}

void BaseAPI::rebootDevice(AsyncWebServerRequest* request) {
  switch (_networkMethodsMap_enum[request->method()]) {
    case GET: {
      request->send(200, MIMETYPE_JSON, "{\"msg\":\"Rebooting Device\"}");
      OpenIrisTasks::ScheduleRestart(2000);
    }
    default: {
      request->send(400, MIMETYPE_JSON, "{\"msg\":\"Invalid Request\"}");
      break;
    }
  }
}

void BaseAPI::factoryReset(AsyncWebServerRequest* request) {
  switch (_networkMethodsMap_enum[request->method()]) {
    case GET: {
      log_d("Factory Reset");
      projectConfig->reset();
      request->send(200, MIMETYPE_JSON, "{\"msg\":\"Factory Reset\"}");
    }
    default: {
      request->send(400, MIMETYPE_JSON, "{\"msg\":\"Invalid Request\"}");
      break;
    }
  }
}

//*********************************************************************************************
//!                                     Camera Command Functions
//*********************************************************************************************

void BaseAPI::setCamera(AsyncWebServerRequest* request) {
  switch (_networkMethodsMap_enum[request->method()]) {
    case GET: {
      // create temporary variables to store the values
      uint8_t temp_camera_framesize = 0;
      uint8_t temp_camera_vflip = 0;
      uint8_t temp_camera_hflip = 0;
      uint8_t temp_camera_quality = 0;
      uint8_t temp_camera_brightness = 0;

      int params = request->params();
      //! Using the else if statements to ensure that the values do not need to
      //! be set in a specific order This means the order of the URL params does
      //! not matter
      for (int i = 0; i < params; i++) {
        AsyncWebParameter* param = request->getParam(i);
        if (param->name() == "framesize") {
          temp_camera_framesize = (uint8_t)param->value().toInt();
        } else if (param->name() == "vflip") {
          temp_camera_vflip = (uint8_t)param->value().toInt();
        } else if (param->name() == "hflip") {
          temp_camera_hflip = (uint8_t)param->value().toInt();
        } else if (param->name() == "quality") {
          temp_camera_quality = (uint8_t)param->value().toInt();
        } else if (param->name() == "brightness") {
          temp_camera_brightness = (uint8_t)param->value().toInt();
        }
      }
      // note: We're passing empty params by design, this is done to reset
      // specific fields
      projectConfig->setCameraConfig(&temp_camera_vflip, &temp_camera_framesize,
                                     &temp_camera_hflip, &temp_camera_quality,
                                     &temp_camera_brightness, true);

      request->send(200, MIMETYPE_JSON,
                    "{\"msg\":\"Done. Camera Settings have been set.\"}");
      break;
    }
    default: {
      request->send(400, MIMETYPE_JSON, "{\"msg\":\"Invalid Request\"}");
      request->redirect("/");
      break;
    }
  }
}

void BaseAPI::restartCamera(AsyncWebServerRequest* request) {
  bool mode = (bool)atoi(request->arg("mode").c_str());
  camera->resetCamera(mode);

  request->send(200, MIMETYPE_JSON,
                "{\"msg\":\"Done. Camera had been restarted.\"}");
}

void BaseAPI::ping(AsyncWebServerRequest* request) {
  request->send(200, MIMETYPE_JSON, "{\"msg\": \"ok\" }");
}

void BaseAPI::save(AsyncWebServerRequest* request) {
  projectConfig->save();
  request->send(200, MIMETYPE_JSON, "{\"msg\": \"ok\" }");
}

void BaseAPI::rssi(AsyncWebServerRequest* request) {
  int rssi = Network_Utilities::getStrength(
      request->getParam("points")->value().toInt());
  char _rssiBuffer[20];
  snprintf(_rssiBuffer, sizeof(_rssiBuffer), "{\"rssi\": %d }", rssi);
  request->send(200, MIMETYPE_JSON, _rssiBuffer);
}
