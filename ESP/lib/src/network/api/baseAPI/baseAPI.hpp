#ifndef BASEAPI_HPP
#define BASEAPI_HPP

//! Warning do not format this file with clang-format or it will break the code

#include <string>
#include <unordered_map>

#include <stdlib_noniso.h>

#define WEBSERVER_H

// hack, we have to include it JUST so the macro executes and defines HTTP_*
// methods cause it's beging included later in streamServer.hpp, but then the
// macro executes too late and tries to redefine the methods, failing. but if we
// don't include it here, then ESPAsyncWebServer fails to compile due to
// WEBSERVER_H which has to be set because if it isn't, ESPAsyncWebServer will
// define the methods and thus the circus continues
#include <http_parser.h>

constexpr int HTTP_ANY = 0b01111111;

#include <Update.h>
#include <esp_int_wdt.h>
#include <esp_task_wdt.h>

#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include "Hash.h"
#include "data/StateManager/StateManager.hpp"
#include "data/config/project_config.hpp"
#include "data/utilities/network_utilities.hpp"
#include "elegantWebpage.h"
#include "io/camera/cameraHandler.hpp"
#include "tasks/tasks.hpp"

class BaseAPI {
 protected:
  std::string api_url;
  bool _authRequired;

  static const char* MIMETYPE_HTML;
  static const char* MIMETYPE_JSON;

 protected:
  /* Commands */
  void setWiFi(AsyncWebServerRequest* request);
  void setWiFiTXPower(AsyncWebServerRequest* request);
  void getJsonConfig(AsyncWebServerRequest* request);
  void factoryReset(AsyncWebServerRequest* request);
  void setDeviceConfig(AsyncWebServerRequest* request);
  void rebootDevice(AsyncWebServerRequest* request);
  void ping(AsyncWebServerRequest* request);
  void save(AsyncWebServerRequest* request);
  void rssi(AsyncWebServerRequest* request);

  /* Camera Handlers */
  void setCamera(AsyncWebServerRequest* request);
  void restartCamera(AsyncWebServerRequest* request);

  /* Route Command types */
  using route_method = void (BaseAPI::*)(AsyncWebServerRequest*);
  typedef std::unordered_map<std::string, route_method> route_t;
  typedef std::unordered_map<std::string, route_t> route_map_t;

  route_t routes;
  route_map_t route_map;

  std::unordered_map<int, std::string> _networkMethodsMap = {
      {0b00000001, "GET"},    {0b00000010, "POST"},  {0b00001000, "PUT"},
      {0b00000100, "DELETE"}, {0b00010000, "PATCH"}, {0b01000000, "OPTIONS"},
  };

  enum RequestMethods {
    GET,
    POST,
    PUT,
    DELETE,
    PATCH,
    OPTIONS,
  };

  std::unordered_map<int, RequestMethods> _networkMethodsMap_enum = {
      {0b00000001, GET},    {0b00000010, POST},  {0b00001000, PUT},
      {0b00000100, DELETE}, {0b00010000, PATCH}, {0b01000000, OPTIONS},
  };

  typedef std::unordered_map<std::string, WebRequestMethodComposite>
      networkMethodsMap_t;

  ProjectConfig& projectConfig;
  /// @brief Local instance of the AsyncWebServer - so that we dont need to use
  /// new and delete
  AsyncWebServer server;
#ifndef SIM_ENABLED
  CameraHandler& camera;
#endif  // SIM_ENABLED

 public:
  BaseAPI(ProjectConfig& projectConfig,
#ifndef SIM_ENABLED
          CameraHandler& camera,
#endif  // SIM_ENABLED
          const std::string& api_url,
#ifndef SIM_ENABLED
          int port = 81
#else
          int port = 80
#endif
  );

  virtual ~BaseAPI();
  virtual void begin();
  void checkAuthentication(AsyncWebServerRequest* request,
                           const char* login,
                           const char* password);
  void beginOTA();
  void notFound(AsyncWebServerRequest* request) const;
};

#endif  // BASEAPI_HPP
