#include "api_server.hpp"

RestAPI::RestAPI(API& api)
    : api(api),
      async_server(80,
                   api.projectConfig,
                   "/control",
                   "/wifimanager",
                   "/openiris"),
      ota(api.projectConfig, async_server),
      server(api.projectConfig, async_server, &ota) {}

RestAPI::~RestAPI() {}

void RestAPI::begin() {
  // handle the WiFi connection state changes
  switch (wifiStateManager.getCurrentState()) {
    case WiFiState_e::WiFiState_Disconnected: {
      break;
    }
    case WiFiState_e::WiFiState_Disconnecting: {
      break;
    }
    case WiFiState_e::WiFiState_ADHOC: {
      setupServer();
      log_d("[SETUP]: Starting API Server");
      break;
    }
    case WiFiState_e::WiFiState_Connected: {
      setupServer();
      log_d("[SETUP]: Starting API Server");
      break;
    }
    case WiFiState_e::WiFiState_Connecting: {
      break;
    }
    case WiFiState_e::WiFiState_Error: {
      break;
    }
  }
}

/**
 * @brief Setup the API Server
 * @note Add all the routes and handlers here
 */
void RestAPI::setupServer() {
  ota.setOTAHandler([this](void) { api.cameraHandler.handleOTA(); });
  server.addAPICommand(
      "/setCamera",
      [this](AsyncWebServerRequest* request) { setCamera(request); });

  server.addAPICommand(
      "/restartCamera",
      [this](AsyncWebServerRequest* request) { restartCamera(request); });

  server.begin();
}

void RestAPI::setCamera(AsyncWebServerRequest* request) {
  switch (async_server._networkMethodsMap_enum[request->method()]) {
    case BaseAPI::GET: {
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

      api.setCamera(temp_camera_vflip, temp_camera_framesize, temp_camera_hflip,
                    temp_camera_quality, temp_camera_brightness, true);

      request->send(200, BaseAPI::MIMETYPE_JSON,
                    "{\"msg\":\"Done. Camera Settings have been set.\"}");
      break;
    }
    default: {
      request->send(400, BaseAPI::MIMETYPE_JSON,
                    "{\"msg\":\"Invalid Request\"}");
      request->redirect("/");
      break;
    }
  }
}

void RestAPI::restartCamera(AsyncWebServerRequest* request) {
  api.restartCamera(request->arg("mode").c_str());
  request->send(200, BaseAPI::MIMETYPE_JSON,
                "{\"msg\":\"Done. Camera had been restarted.\"}");
}
