#include "stream_server.hpp"

constexpr static const char* STREAM_CONTENT_TYPE =
    "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
constexpr static const char* STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
constexpr static const char* STREAM_PART =
    "Content-Type: image/jpeg\r\nContent-Length: %u\r\nX-Timestamp: "
    "%d.%06d\r\n\r\n";

esp_err_t StreamHelpers::stream(httpd_req_t* req) {
  long last_request_time = 0;
  camera_fb_t* fb = NULL;
  struct timeval _timestamp;

  esp_err_t res = ESP_OK;

  size_t _jpg_buf_len = 0;
  uint8_t* _jpg_buf = NULL;

  char* part_buf[256];

  static int64_t last_frame = 0;
  if (!last_frame)
    last_frame = esp_timer_get_time();

  res = httpd_resp_set_type(req, STREAM_CONTENT_TYPE);
  if (res != ESP_OK)
    return res;

  httpd_resp_set_hdr(
      req,
      "Access-Control-Allow-Origin; Content-Type: multipart/x-mixed-replace; "
      "boundary=123456789000000000000987654321\r\n",
      "*");
  httpd_resp_set_hdr(req, "X-Framerate", "60");

  while (true) {
    fb = esp_camera_fb_get();
    if (!fb) {
      log_e("Camera capture failed with response: %s", esp_err_to_name(res));
      res = ESP_FAIL;
    } else {
      _timestamp.tv_sec = fb->timestamp.tv_sec;
      _timestamp.tv_usec = fb->timestamp.tv_usec;
      _jpg_buf_len = fb->len;
      _jpg_buf = fb->buf;
    }
    if (res == ESP_OK)
      res =
          httpd_resp_send_chunk(req, STREAM_BOUNDARY, strlen(STREAM_BOUNDARY));
    if (res == ESP_OK) {
      size_t hlen = snprintf((char*)part_buf, 128, STREAM_PART, _jpg_buf_len,
                             _timestamp.tv_sec, _timestamp.tv_usec);
      res = httpd_resp_send_chunk(req, (const char*)part_buf, hlen);
    }
    if (res == ESP_OK)
      res = httpd_resp_send_chunk(req, (const char*)_jpg_buf, _jpg_buf_len);
    if (fb) {
      esp_camera_fb_return(fb);
      fb = NULL;
      _jpg_buf = NULL;
    } else if (_jpg_buf) {
      free(_jpg_buf);
      _jpg_buf = NULL;
    }
    if (res != ESP_OK)
      break;
    long request_end = millis();
    long latency = (request_end - last_request_time);
    last_request_time = request_end;
    log_d("Size: %uKB, Time: %ums (%ifps)\n", _jpg_buf_len / 1024, latency,
          1000 / latency);
  }
  last_frame = 0;
  return res;
}

StreamServer::StreamServer(const int STREAM_PORT)
    : STREAM_SERVER_PORT(STREAM_PORT) {}

int StreamServer::startStreamServer() {
  // WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //! Turn-off the 'brownout
  // detector'
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  config.stack_size = 20480;
  config.max_uri_handlers = 1;
  config.server_port = this->STREAM_SERVER_PORT;
  config.ctrl_port = this->STREAM_SERVER_PORT;
  config.stack_size = 20480;

  httpd_uri_t stream_page = {.uri = "/",
                             .method = HTTP_GET,
                             .handler = &StreamHelpers::stream,
                             .user_ctx = nullptr};

  int status = httpd_start(&camera_stream, &config);

  if (status != ESP_OK)
    return -1;
  else {
    httpd_register_uri_handler(camera_stream, &stream_page);
    Serial.println("Stream server initialized");
    switch (wifiStateManager.getCurrentState()) {
      case WiFiState_e::WiFiState_ADHOC:
        Serial.printf("\n\rThe stream is under: http://%s:%i\n\r",
                      WiFi.softAPIP().toString().c_str(),
                      this->STREAM_SERVER_PORT);
        break;
      default:
        Serial.printf("\n\rThe stream is under: http://%s:%i\n\r",
                      WiFi.localIP().toString().c_str(),
                      this->STREAM_SERVER_PORT);
        break;
    }
    return 0;
  }
}
