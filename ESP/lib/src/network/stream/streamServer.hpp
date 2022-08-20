#pragma once
#ifndef STREAM_SERVER_HPP
#define STREAM_SERVER_HPP
#define PART_BOUNDARY "123456789000000000000987654321"
#include <Arduino.h>
#include "esp_camera.h"
#include "esp_http_server.h"

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
  StreamServer(int STREAM_PORT) : STREAM_SERVER_PORT(STREAM_PORT) {}
  int startStreamServer();
};

#endif // STREAM_SERVER_HPP
