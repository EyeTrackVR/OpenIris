#pragma once

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

public:
  APIServer();
  void startAPIServer();
};
