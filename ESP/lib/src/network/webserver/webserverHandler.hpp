#pragma once
#include "io/camera/cameraHandler.hpp"

#define WEBSERVER_H
#define HTTP_ANY 0b01111111
#define HTTP_GET 0b00000001

#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "network/WifiHandler/WifiHandler.hpp"

class APIServer
{
private:
    void command_handler(AsyncWebServerRequest *request);

    /* I think we should make these unique_ptr */
    // std::unique_ptr<AsyncWebServer> server;
    // std::unique_ptr<CameraHandler> cameraHandler;
    AsyncWebServer *server;
    CameraHandler *cameraHandler;
    WiFiHandler *network;

public:
    APIServer(int CONTROL_PORT, CameraHandler *cameraHandler, WiFiHandler *network);
    void startAPIServer();
};
