#pragma once
#ifndef XWEBSERVERHANDLER_HPP
#define XWEBSERVERHANDLER_HPP

#include "network/api/baseAPI/baseAPI.hpp"

class APIServer : public BaseAPI {
 public:
  APIServer(
            AsyncWebServer &server,
            ProjectConfig& projectConfig,
#ifndef SIM_ENABLED
            CameraHandler& camera,
#endif  // SIM_ENABLED
            const std::string& api_url);

  virtual ~APIServer();
  void setup();
  void setupServer();
  void addRouteMap(const std::string& index,
                   route_t route,
                   std::vector<std::string>& indexes);
  void handleRequest(AsyncWebServerRequest* request);

 public:
  std::vector<std::string> indexes;
};
#endif  // WEBSERVERHANDLER_HPP
