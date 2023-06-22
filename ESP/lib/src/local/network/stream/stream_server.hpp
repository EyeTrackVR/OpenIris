#pragma once
#ifndef STREAM_SERVER_HPP
#define STREAM_SERVER_HPP
#define PART_BOUNDARY "123456789000000000000987654321"
#include <Arduino.h>
#include <WiFi.h>
#include <data/statemanager/state_manager.hpp>

// Camera includes
#include "esp_camera.h"
#include "esp_http_server.h"
#include "esp_timer.h"
#include "fb_gfx.h"
#include "img_converters.h"

namespace StreamHelpers
{
    esp_err_t stream(httpd_req_t *req);
}
class StreamServer
{
private:
    httpd_handle_t camera_stream = nullptr;
    int STREAM_SERVER_PORT;

public:
    StreamServer(const int STREAM_PORT = 80);
    int startStreamServer();
};

#endif // STREAM_SERVER_HPP
