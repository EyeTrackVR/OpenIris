#pragma once
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "io/camera/cameraHandler.hpp"

#define WEBSERVER_H
#define HTTP_ANY 0b01111111
#define HTTP_GET 0b00000001

class APIServer
{
private:
    void command_handler(AsyncWebServerRequest *request);

    /* I think we should make these unique_ptr */
    //std::unique_ptr<AsyncWebServer> server;
    //std::unique_ptr<CameraHandler> cameraHandler;
    AsyncWebServer *server;
    CameraHandler *cameraHandler;

public:
    APIServer(int CONTROL_PORT, CameraHandler *cameraHandler);
    void startAPIServer();
};
