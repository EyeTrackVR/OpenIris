#include <Arduino.h>
#include "GlobalVars.h"
#include "httpdHandler.h"

constexpr static char* STREAM_CONTENT_TYPE  = "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
constexpr static char* STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
constexpr static char* STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length: %u\r\nX-Timestamp: %d.%06d\r\n\r\n";

esp_err_t OpenIris::HTTPHelpers::stream_handler(httpd_req_t *req) {
    camera_fb_t *fb = NULL;
    struct timeval _timestamp;

    esp_err_t res = ESP_OK;

    size_t _jpg_buf_len = 0;
    uint8_t *_jpg_buf = NULL;

    char *part_buf[128];

    static int64_t last_frame = 0;
    if (!last_frame)
        last_frame = esp_timer_get_time();

    res = httpd_resp_set_type(req, STREAM_CONTENT_TYPE);
    if (res != ESP_OK)
        return res;

    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin; Content-Type: multipart/x-mixed-replace; boundary=123456789000000000000987654321\r\n", "*");
    httpd_resp_set_hdr(req, "X-Framerate", "60");

    while (true) {
        fb = esp_camera_fb_get();
        if (!fb){
            ESP_LOGE(TAG, "Camera capture failed");
            res = ESP_FAIL;
        }
        else{
            _timestamp.tv_sec = fb->timestamp.tv_sec;
            _timestamp.tv_usec = fb->timestamp.tv_usec;
                if (fb->format != PIXFORMAT_JPEG){
                    bool jpeg_converted = frame2jpg(fb, 80, &_jpg_buf, &_jpg_buf_len);
                    esp_camera_fb_return(fb);
                    fb = NULL;
                    if (!jpeg_converted){
                        ESP_LOGE(TAG, "JPEG compression failed");
                        res = ESP_FAIL;
                    }
                }
                else{
                    _jpg_buf_len = fb->len;
                    _jpg_buf = fb->buf;
                }
        }
        if (res == ESP_OK){
            res = httpd_resp_send_chunk(req, STREAM_BOUNDARY, strlen(STREAM_BOUNDARY));
        }
        if (res == ESP_OK){
            size_t hlen = snprintf((char *)part_buf, 128, STREAM_PART, _jpg_buf_len, _timestamp.tv_sec, _timestamp.tv_usec);
            res = httpd_resp_send_chunk(req, (const char *)part_buf, hlen);
        }
        if (res == ESP_OK){
            res = httpd_resp_send_chunk(req, (const char *)_jpg_buf, _jpg_buf_len);
        }
        if (fb){
            esp_camera_fb_return(fb);
            fb = NULL;
            _jpg_buf = NULL;
        }
        else if (_jpg_buf){
            free(_jpg_buf);
            _jpg_buf = NULL;
        }
        if (res != ESP_OK){
            break;
        }
    }
    last_frame = 0;
    return res;
}

esp_err_t OpenIris::HTTPHelpers::parse_get(httpd_req_t *req, char **obuf) {
    char *buf = nullptr;
    size_t buf_len = 0;

    buf_len = httpd_req_get_url_query_len(req) + 1;
    if (buf_len > 1) {
        buf = (char *)malloc(buf_len);
        if (!buf) {
            httpd_resp_send_500(req);
            return ESP_FAIL;
        }
        if (httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK) {
            *obuf = buf;
            return ESP_OK;
        }
        free(buf);
    }
    httpd_resp_send_404(req);
    return ESP_FAIL;
}

esp_err_t OpenIris::HTTPHelpers::command_handler(httpd_req_t *req) {
    char *buf = nullptr;
    char variable[32];
    char value[32];

    if (parse_get(req, &buf) != ESP_OK)
        return ESP_FAIL;
    if (httpd_query_key_value(buf, "var", variable, sizeof(variable)) != ESP_OK ||
        httpd_query_key_value(buf, "val", value, sizeof(value)) != ESP_OK) {
        free(buf);
        httpd_resp_send_404(req);
        return ESP_FAIL;
    }
    free(buf);

    int val = atoi(value);
    int res = 0;
    if (!strcmp(variable, "framesize"))
        res = cameraHandler.setCameraResolution((framesize_t)val);
    else if (!strcmp(variable, "hmirror"))
        res = cameraHandler.setHFlip(val);
    else if (!strcmp(variable, "vflip"))
        res = cameraHandler.setVFlip(val);
    else
        res = -1; // invalid command

    if (res < 0)
        return httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Command not supported");

    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    return httpd_resp_send(req, nullptr, 0);
}

int OpenIris::HTTPDHandler::startStreamServer(){
    Serial.println("Setting up the server");

    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.max_uri_handlers = 3;
    config.server_port = STREAM_SERVER_PORT;
    config.ctrl_port = STREAM_SERVER_PORT;

    httpd_uri_t control_page = {
        .uri = "/control/",
        .method = HTTP_GET,
        .handler = &OpenIris::HTTPHelpers::command_handler,
        .user_ctx = nullptr
    };

    httpd_uri_t stream_page = {
        .uri = "/",
        .method = HTTP_GET,
        .handler = &OpenIris::HTTPHelpers::stream_handler,
        .user_ctx = nullptr
    };

    int streamer_status = httpd_start(&control_httpd, &config);

    config.server_port = CONTROL_SERVER_PORT;
    config.ctrl_port = CONTROL_SERVER_PORT;

    int cmd_controller_status = httpd_start(&camera_httpd, &config);

    if (streamer_status != ESP_OK || cmd_controller_status != ESP_OK)
        return -1;
    else {
        httpd_register_uri_handler(control_httpd, &control_page);
        httpd_register_uri_handler(camera_httpd, &stream_page);
        Serial.println("Server is ready to serve!");
        return 0;
    }
}