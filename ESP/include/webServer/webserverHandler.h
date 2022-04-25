#pragma once
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "esp_camera.h"

namespace OpenIris
{
  class HTTPDHandler
  {
  private:
    void command_handler(AsyncWebServerRequest *request);
    void stream_handler(AsyncWebServerRequest *request);
    void roi_crop_command_handler(AsyncWebServerRequest *request);
    AsyncWebServer *server;

  public:
    HTTPDHandler();
    void startStreamServer();
  };
}