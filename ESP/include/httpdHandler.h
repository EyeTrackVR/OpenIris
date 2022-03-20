#pragma once
#define PART_BOUNDARY "123456789000000000000987654321"

#include "esp_camera.h"
#include "esp_http_server.h"

namespace OpenIris{
    namespace HTTPHelpers {
        esp_err_t stream_handler(httpd_req_t *req);
        esp_err_t parse_get(httpd_req_t *req, char **obuf);
        esp_err_t command_handler(httpd_req_t *req);
    }

    class HTTPDHandler{
    private:
        httpd_handle_t camera_httpd = nullptr;
        httpd_handle_t control_httpd = nullptr;
    public:
        int startStreamServer();
    };
}