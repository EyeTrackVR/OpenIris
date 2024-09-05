#ifndef BASEAPI_HPP
#define BASEAPI_HPP

//! Warning do not format this file with clang-format or it will break the code

#include <string>
#include <unordered_map>

#include <stdlib_noniso.h>

#define WEBSERVER_H

/* #define XHTTP_GET 0b00000001;
#define XHTTP_POST 0b00000010;
#define XHTTP_DELETE 0b00000100;
#define XHTTP_PUT 0b00001000;
#define XHTTP_PATCH 0b00010000;
#define XHTTP_HEAD 0b00100000;
#define XHTTP_OPTIONS 0b01000000;
#define XHTTP_ANY 0b01111111; */

// constexpr int HTTP_GET = 0b00000001;
constexpr int HTTP_ANY = 0b01111111;

#include <Update.h>
#include <esp_int_wdt.h>
#include <esp_task_wdt.h>

#include <AsyncTCP.h>

#include <ESPAsyncWebServer.h>
#include <FS.h>
#include "Hash.h"

#include "data/utilities/network_utilities.hpp"
#include "tasks/tasks.hpp"

#include "data/StateManager/StateManager.hpp"
#include "data/config/project_config.hpp"
#include "elegantWebpage.h"
#include "io/camera/cameraHandler.hpp"

class BaseAPI {
 protected:
  bool _authRequired;

  static const char* MIMETYPE_HTML;
  /* static const char *MIMETYPE_CSS; */
  /* static const char *MIMETYPE_JS; */
  /* static const char *MIMETYPE_PNG; */
  /* static const char *MIMETYPE_JPG; */
  /* static const char *MIMETYPE_ICO; */
  static const char* MIMETYPE_JSON;

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

  /// @brief Local instance of the AsyncWebServer - so that we dont need to use
  /// new and delete
  AsyncWebServer server;
  ProjectConfig& projectConfig;

#ifndef SIM_ENABLED
  CameraHandler& camera;
#endif  // SIM_ENABLED
  std::string api_url;

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
