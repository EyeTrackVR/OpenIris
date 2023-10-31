#ifndef API_SERVER_HPP
#define API_SERVER_HPP
#include <EasyNetworkManager.hpp>
#include <local/data/api/api.hpp>

class RestAPI {
 private:
  API& api;
  AsyncServer_t async_server;
  AsyncOTA ota;
  APIServer server;
  void setupServer();
  void setCamera(AsyncWebServerRequest* request);
  void restartCamera(AsyncWebServerRequest* request);

 public:
  RestAPI(API& api);
  virtual ~RestAPI();
  void begin();
};

#endif  // API_HPP
