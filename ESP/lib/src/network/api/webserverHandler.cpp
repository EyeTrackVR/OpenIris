#include "webserverHandler.hpp"

//*********************************************************************************************
//!                                     API Server
//*********************************************************************************************

APIServer::APIServer(ProjectConfig* projectConfig,
                     CameraHandler* camera,
                     StateManager<WiFiState_e>* wiFiStateManager,
                     const std::string& api_url)
    : BaseAPI(projectConfig, camera, wiFiStateManager, api_url) {}

APIServer::~APIServer() {}

void APIServer::setup() {
  log_d("Initializing REST API Server");
  this->setupServer();
  BaseAPI::begin();

  char buffer[1000];
  snprintf(buffer, sizeof(buffer),
           "^\\%s\\/([a-zA-Z0-9]+)\\/command\\/([a-zA-Z0-9]+)$",
           this->api_url.c_str());
  log_d("API URL: %s", buffer);

  this->server.on(buffer, 0b01111111, [&](AsyncWebServerRequest* request) {
    handleRequest(request);
  });

  // Note: Start OTA after all routes have been added
#ifndef SIM_ENABLED
  //this->_authRequired = true;
#endif  // SIM_ENABLED
  this->beginOTA();
  server.begin();
}

void APIServer::setupServer() {
  routes.emplace("wifi", &APIServer::setWiFi);
  routes.emplace("resetConfig", &APIServer::factoryReset);
  routes.emplace("setDevice", &APIServer::setDeviceConfig);
  routes.emplace("rebootDevice", &APIServer::rebootDevice);
  routes.emplace("getStoredConfig", &APIServer::getJsonConfig);
  routes.emplace("setTxPower", &APIServer::setWiFiTXPower);
  // Camera Routes
  routes.emplace("setCamera", &APIServer::setCamera);
  routes.emplace("restartCamera", &APIServer::restartCamera);
  routes.emplace("ping", &APIServer::ping);
  routes.emplace("save", &APIServer::save);
  routes.emplace("wifiStrength", &APIServer::rssi);

  //! reserve enough memory for all routes - must be called after adding routes
  //! and before adding routes to route_map
  indexes.reserve(routes.size());  // this is done to avoid reallocation of
                                   // memory and copying of data
  addRouteMap("builtin", routes,
              indexes);  // add new route map to the route_map
}

void APIServer::findParam(AsyncWebServerRequest* request,
                          const char* param,
                          std::string& value) {
  if (request->hasParam(param)) {
    value = request->getParam(param)->value().c_str();
  }
}

/**
 * @brief Add a command handler to the API
 *
 * @param index
 * @param funct
 * @param indexes \c std::vector<std::string> a list of the routes of the
 * command handlers
 *
 * @return void
 *
 */
void APIServer::addRouteMap(const std::string& index,
                            route_t route,
                            std::vector<std::string>& indexes) {
  route_map.emplace(index, route);

  for (const auto& key : route) {
    indexes.emplace_back(key.first);  // add the route to the list of routes -
                                      // use emplace_back to avoid copying
  }
}

void APIServer::handleRequest(AsyncWebServerRequest* request) {
  try {
    // Get the route
    log_i("Request URL: %s", request->url().c_str());
    log_i("Request: %s", request->pathArg(0).c_str());
    log_i("Request: %s", request->pathArg(1).c_str());

    auto it_map = route_map.find(request->pathArg(0).c_str());
    auto it_method = it_map->second.find(request->pathArg(1).c_str());

    if (it_map != route_map.end()) {
      if (it_method != it_map->second.end()) {
        log_d("We are trying to execute the function");
        (*this.*(it_method->second))(request);
      } else {
        log_e("Invalid Command");
        request->send(400, MIMETYPE_JSON, "{\"msg\":\"Invalid Command\"}");
        return;
      }
    } else {
      log_e("Invalid Map Index");
      request->send(400, MIMETYPE_JSON, "{\"msg\":\"Invalid Map Index\"}");
      return;
    }
  } catch (...) {
    log_e("Error handling request");
  }
}
