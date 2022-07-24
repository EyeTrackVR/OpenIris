#pragma once
#include "../../io/camera/cameraHandler.hpp"
#define WEBSERVER_H
#define HTTP_ANY 0b01111111
#define HTTP_GET 0b00000001
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

class APIServer
{
private:
  void command_handler(AsyncWebServerRequest *request);
  AsyncWebServer *server;
  CameraHandler *cameraHandler;

public:
  APIServer(int CONTROL_PORT, CameraHandler *cameraHandler);
  void startAPIServer();
};
