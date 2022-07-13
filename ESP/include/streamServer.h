#pragma once
#define PART_BOUNDARY "123456789000000000000987654321"
#include <Arduino.h>
#include "esp_camera.h"
#include "esp_http_server.h"

namespace OpenIris
{
  namespace StreamHelpers
  {
    esp_err_t stream(httpd_req_t *req);
  }
  class StreamServer
  {

  private:
    httpd_handle_t camera_stream = nullptr;

  public:
    int startStreamServer();
  };
}