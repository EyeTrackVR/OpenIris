#pragma once
#ifndef XWEBSERVERHANDLER_HPP
#define XWEBSERVERHANDLER_HPP

#include "network/api/baseAPI/baseAPI.hpp"

class APIServer : public BaseAPI {
 public:
  APIServer(ProjectConfig* projectConfig,
            CameraHandler* camera,
            const std::string& api_url,
            int CONTROL_PORT = 81);

  virtual ~APIServer();
  void begin();
  void setupServer();
  void addRouteMap(const std::string& index,
                   route_t route,
                   std::vector<std::string>& indexes);
  void handleRequest(AsyncWebServerRequest* request);

 public:
  std::vector<std::string> indexes;
};
#endif  // WEBSERVERHANDLER_HPP
