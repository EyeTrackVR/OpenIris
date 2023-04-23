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

BaseAPI::BaseAPI(
                 AsyncWebServer &server,
                 ProjectConfig& projectConfig,
#ifndef SIM_ENABLED
                 CameraHandler& camera,
#endif  // SIM_ENABLED
                 const std::string& api_url)
    : server(server),
      projectConfig(projectConfig),
#ifndef SIM_ENABLED
      camera(camera),
#endif  // SIM_ENABLED
      api_url(api_url) {
}

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
          networkName.assign(param->value().c_str());
        } else if (param->name() == "ssid") {
          ssid.assign(param->value().c_str());
        } else if (param->name() == "password") {
          password.assign(param->value().c_str());
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
      projectConfig.setWifiConfig(networkName, ssid, password, channel, power,
                                   adhoc, true);

      /* if (WiFiStateManager->getCurrentState() ==
      WiFiState_e::WiFiState_ADHOC)
      {
              projectConfig.setAPWifiConfig(ssid, password, &channel, adhoc,
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
      projectConfig.deleteWifiConfig(request->arg("networkName").c_str(),
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
      auto networksConfigs = projectConfig.getWifiConfigs();
      for (auto& networkConfig : networksConfigs) {
        wifiConfigSerialized += networkConfig.toRepresentation();

        if (&networkConfig != &networksConfigs.back())
          wifiConfigSerialized += ",";
      }
      wifiConfigSerialized += "]";

      std::string json = Helpers::format_string(
          "{%s, %s, %s, %s, %s}",
          projectConfig.getDeviceConfig().toRepresentation().c_str(),
          projectConfig.getCameraConfig().toRepresentation().c_str(),
          wifiConfigSerialized.c_str(),
          projectConfig.getMDNSConfig().toRepresentation().c_str(),
          projectConfig.getAPWifiConfig().toRepresentation().c_str());
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
      std::string ota_login;
      int ota_port;

      for (int i = 0; i < params; i++) {
        AsyncWebParameter* param = request->getParam(i);
        if (param->name() == "hostname") {
          std::string result = param->value().c_str();

          // Convert to lower case using lambda - to  force lower case
          std::for_each(result.begin(), result.end(),
                        [](char& c) { c = ::tolower(c); });

          hostname.assign(result);
        } else if (param->name() == "service") {
          service.assign(param->value().c_str());
        } else if (param->name() == "ota_port") {
          ota_port = atoi(param->value().c_str());
        } else if (param->name() == "ota_login") {
          ota_login.assign(param->value().c_str());
        } else if (param->name() == "ota_password") {
          ota_password.assign(param->value().c_str());
        }
      }
      // note: We're passing empty params by design, this is done to reset
      // specific fields
      projectConfig.setDeviceConfig(ota_login, ota_password, ota_port, true);
      projectConfig.setMDNSConfig(hostname, service, true);
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
      projectConfig.setWiFiTxPower(txPower, true);
      projectConfig.wifiTxPowerConfigSave();
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
      projectConfig.setWiFiTxPower(txPower, true);
      projectConfig.wifiTxPowerConfigSave();
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
      projectConfig.reset();
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
#ifndef SIM_ENABLED
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
      projectConfig.setCameraConfig(temp_camera_vflip, temp_camera_framesize,
                                     temp_camera_hflip, temp_camera_quality,
                                     temp_camera_brightness, true);

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
  camera.resetCamera(mode);

  request->send(200, MIMETYPE_JSON,
                "{\"msg\":\"Done. Camera had been restarted.\"}");
}
#endif // SIM_ENABLED

//*********************************************************************************************
//!                                     General Command Functions
//*********************************************************************************************

void BaseAPI::ping(AsyncWebServerRequest* request) {
  request->send(200, MIMETYPE_JSON, "{\"msg\": \"ok\" }");
}

void BaseAPI::save(AsyncWebServerRequest* request) {
  projectConfig.save();
  request->send(200, MIMETYPE_JSON, "{\"msg\": \"ok\" }");
}

void BaseAPI::rssi(AsyncWebServerRequest* request) {
  int rssi = Network_Utilities::getStrength(
      request->getParam("points")->value().toInt());
  char _rssiBuffer[20];
  snprintf(_rssiBuffer, sizeof(_rssiBuffer), "{\"rssi\": %d }", rssi);
  request->send(200, MIMETYPE_JSON, _rssiBuffer);
}

//*********************************************************************************************
//!                                     OTA Command Functions
//*********************************************************************************************

void BaseAPI::checkAuthentication(AsyncWebServerRequest* request,
                                  const char* login,
                                  const char* password) {
  log_d("[DEBUG] Free Heap: %d", ESP.getFreeHeap());
  if (_authRequired) {
    log_d("[DEBUG] Free Heap: %d", ESP.getFreeHeap());
    log_i("Auth required");
    log_d("[DEBUG] Free Heap: %d", ESP.getFreeHeap());
    if (!request->authenticate(login, password, NULL, false)) {
      log_d("[DEBUG] Free Heap: %d", ESP.getFreeHeap());
      return request->requestAuthentication(NULL, false);
    }
  }
}

void BaseAPI::beginOTA() {
  // NOTE: Code adapted from: https://github.com/ayushsharma82/AsyncElegantOTA/

  auto device_config = projectConfig.getDeviceConfig();
  auto mdns_config = projectConfig.getMDNSConfig();

  if (device_config.OTAPassword.empty()) {
    log_e(
        "Password is empty, you need to provide a password in order to setup "
        "the OTA server");
    return;
  }

  log_i("[OTA Server]: Initializing OTA Server");
  log_i(
      "[OTA Server]: Navigate to http://%s.local:81/update to update the "
      "firmware",
      mdns_config.hostname.c_str());

  log_d("[OTA Server]: Username: %s, Password: %s",
        device_config.OTALogin.c_str(), device_config.OTAPassword.c_str());
  log_d("[DEBUG] Free Heap: %d", ESP.getFreeHeap());
  const char* login = device_config.OTALogin.c_str();
  const char* password = device_config.OTAPassword.c_str();
  log_d("[DEBUG] Free Heap: %d", ESP.getFreeHeap());

  // Note: HTTP_GET
  server.on(
      "/update/identity", 0b00000001, [&](AsyncWebServerRequest* request) {
        checkAuthentication(request, login, password);

        String _id = String((uint32_t)ESP.getEfuseMac(), HEX);
        _id.toUpperCase();
        request->send(200, "application/json",
                      "{\"id\": \"" + _id + "\", \"hardware\": \"ESP32\"}");
      });

  // Note: HTT_GET
  server.on("/update", 0b00000001, [&](AsyncWebServerRequest* request) {
    log_d("[DEBUG] Free Heap: %d", ESP.getFreeHeap());
    checkAuthentication(request, login, password);

    // turn off the camera and stop the stream
    esp_camera_deinit();                // deinitialize the camera driver
    digitalWrite(PWDN_GPIO_NUM, HIGH);  // turn power off to camera module

    AsyncWebServerResponse* response = request->beginResponse_P(
        200, "text/html", ELEGANT_HTML, ELEGANT_HTML_SIZE);
    response->addHeader("Content-Encoding", "gzip");
    request->send(response);
  });
  // Note: HTT_POST
  server.on(
      "/update", 0b00000010,
      [&](AsyncWebServerRequest* request) {
        checkAuthentication(request, login, password);
        // the request handler is triggered after the upload has finished...
        // create the response, add header, and send response
        AsyncWebServerResponse* response = request->beginResponse(
            (Update.hasError()) ? 500 : 200, "text/plain",
            (Update.hasError()) ? "FAIL" : "OK");
        response->addHeader("Connection", "close");
        response->addHeader("Access-Control-Allow-Origin", "*");
        request->send(response);
        this->save(request);
      },
      [&](AsyncWebServerRequest* request, String filename, size_t index,
          uint8_t* data, size_t len, bool final) {
        // Upload handler chunks in data
        checkAuthentication(request, login, password);

        if (!index) {
          if (!request->hasParam("MD5", true)) {
            return request->send(400, "text/plain", "MD5 parameter missing");
          }

          if (!Update.setMD5(request->getParam("MD5", true)->value().c_str())) {
            return request->send(400, "text/plain", "MD5 parameter invalid");
          }
          int cmd = (filename == "filesystem") ? U_SPIFFS : U_FLASH;
          if (!Update.begin(UPDATE_SIZE_UNKNOWN,
                            cmd)) {  // Start with max available size
            Update.printError(Serial);
            return request->send(400, "text/plain", "OTA could not begin");
          }
        }

        // Write chunked data to the free sketch space
        if (len) {
          if (Update.write(data, len) != len) {
            return request->send(400, "text/plain", "OTA could not begin");
          }
        }

        if (final) {  // if the final flag is set then this is the last frame of
                      // data
          if (!Update.end(
                  true)) {  // true to set the size to the current progress
            Update.printError(Serial);
            return request->send(400, "text/plain", "Could not end OTA");
          }
        } else {
          return;
        }
      });
}
