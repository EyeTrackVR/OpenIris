#pragma once
#ifndef XWEBSERVERHANDLER_HPP
#define XWEBSERVERHANDLER_HPP
#include <unordered_map>
#include <string>

#define WEBSERVER_H

#define HTTP_GET 0b00000001
#define HTTP_POST 0b00000010
#define HTTP_DELETE 0b00000100
#define HTTP_PUT 0b00001000
#define HTTP_PATCH 0b00010000
#define HTTP_HEAD 0b00100000
#define HTTP_OPTIONS 0b01000000
#define HTTP_ANY 0b01111111

#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include "mbedtls/md.h"
#include "io/camera/cameraHandler.hpp"
#include "network/WifiHandler/WifiHandler.hpp"

class APIServer
{
private:
    void command_handler(AsyncWebServerRequest *request);

    AsyncWebServer *server;
    CameraHandler *cameraHandler;
    WiFiHandler *network;

    /* Commands */
    void setSSID(const char *value);
    void setPass(const char *value);
    void setChannel(const char *value);

    void setDataJson(AsyncWebServerRequest *request);
    void setConfigJson(AsyncWebServerRequest *request);
    void setSettingsJson(AsyncWebServerRequest *request);

    void factoryReset();
    void rebootDevice();

    typedef std::function<void(const char *)> wifi_conf_function;
    typedef std::function<void(void)> function;
    typedef std::function<void(AsyncWebServerRequest *)> function_w_request;

    typedef std::unordered_map<std::string, function> command_map_funct_t;
    typedef std::unordered_map<std::string, wifi_conf_function> command_map_wifi_conf_t;
    typedef std::unordered_map<std::string, function_w_request> command_map_json_t;

    command_map_funct_t command_map_funct;
    command_map_wifi_conf_t command_map_wifi_conf;
    command_map_json_t command_map_json;

    static const char *MIMETYPE_HTML;
    /* static const char *MIMETYPE_CSS; */
    /* static const char *MIMETYPE_JS; */
    /* static const char *MIMETYPE_PNG; */
    /* static const char *MIMETYPE_JPG; */
    /* static const char *MIMETYPE_ICO; */
    static const char *MIMETYPE_JSON;
    static bool ssid_write;
    static bool pass_write;
    static bool channel_write;

    struct LocalWifiConfig
    {
        std::string ssid;
        std::string pass;
        uint8_t channel;
        bool adhoc;
    };

    struct WifiConfig
    {
        std::vector<LocalWifiConfig> local_WifiConfig;
    };

    WifiConfig wifiConfig;

public:
    APIServer(int CONTROL_PORT, CameraHandler *cameraHandler, WiFiHandler *network);
    void begin();
    void startAPIServer();
    void triggerWifiConfigWrite();
    void findParam(AsyncWebServerRequest *request, const char *param, String &value);

    class API_Utilities
    {
    public:
        API_Utilities();
        void notFound(AsyncWebServerRequest *request);
        void my_delay(volatile long delay_time);
        std::string shaEncoder(std::string data);
        std::unordered_map<WebRequestMethodComposite, std::string> _networkMethodsMap = {
            {HTTP_GET, "GET"},
            {HTTP_POST, "POST"},
            {HTTP_PUT, "PUT"},
            {HTTP_DELETE, "DELETE"},
            {HTTP_PATCH, "PATCH"},
            {HTTP_OPTIONS, "OPTIONS"},
        };
    };
};
extern APIServer::API_Utilities api_utilities;
#endif // WEBSERVERHANDLER_HPP
