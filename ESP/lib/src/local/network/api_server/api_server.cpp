#include "api_server.hpp"

RestAPI::RestAPI(ProjectConfig& projectConfig, OpenIrisConfig& configManager)
    : projectConfig(projectConfig),
      configManager(configManager),
      server(80, projectConfig, "/control", "/wifimanager", "/openiris") {}

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
  server.addAPICommand("/setCamera", [this](AsyncWebServerRequest* request) {

  });

  server.begin();
}


