#pragma onc
#ifndef WEBSERVERHANDLER_HPP
#define WEBSERVERHANDLER_HPP
#include <unordered_map>
#include <string>
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

    AsyncWebServer *server;
    CameraHandler *cameraHandler;
    WiFiHandler *network;

    enum command_func
    {
        FRAME_SIZE,
        HMIRROR,
        VFLIP,
#if ENABLE_ADHOC
        AP_SSID,
        AP_PASSWORD,
        AP_CHANNEL,
#else
        SSID,
        PASSWORD,
        CHANNEL,
#endif // ENABLE_ADHOC
    };

    static std::unordered_map<std::string, command_func> command_map;

public:
    APIServer(int CONTROL_PORT, CameraHandler *cameraHandler, WiFiHandler *network);
    void begin();
    void startAPIServer();
    void findParam(AsyncWebServerRequest *request, const char *param, String &value);
};
#endif // WEBSERVERHANDLER_HPP
