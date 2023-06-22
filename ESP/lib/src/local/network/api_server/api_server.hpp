#ifndef API_SERVER_HPP
#define API_SERVER_HPP
#include <EasyNetworkManager.hpp>
#include <local/data/statemanager/state_manager.hpp>
#include <local/data/config/config.hpp>
class RestAPI {
 private:
  APIServer server;
  ProjectConfig& projectConfig;
  OpenIrisConfig& configManager;
  void setupServer();

 public:
  RestAPI(ProjectConfig& projectConfig, OpenIrisConfig& configManager);
  virtual ~RestAPI();
  void begin();
};

#endif  // API_HPP
