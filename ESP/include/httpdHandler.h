#include "esp_camera.h"
#include "esp_http_server.h"

namespace HttpdHandler{
    esp_err_t stream_handler(httpd_req_t *req);
    esp_err_t parse_get(httpd_req_t *req, char **obuf);
    esp_err_t command_handler(httpd_req_t *req);
    int startStreamServer();
}