#include "streamServer.hpp"

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

  httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
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
    : STREAM_SERVER_PORT(STREAM_PORT) {
  memcpy(initial_packet_buffer, ETVR_HEADER, sizeof(ETVR_HEADER));
}

int StreamServer::startStreamServer() {
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

bool StreamServer::startUDPStreamServer() {
  socket = AsyncUDP();
  return socket.listen(this->STREAM_SERVER_PORT + 1);
}

void StreamServer::sendUDPFrame() {
  ///////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////
  //   TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO
  //
  //   ADD PROTOCOL VERSION
  //
  //   TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO
  ///////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////

  if (!last_frame)
    last_frame = esp_timer_get_time();

  size_t len = 0;
  uint8_t* buf = NULL;

  auto fb = esp_camera_fb_get();
  if (fb) {
    len = fb->len;
    buf = fb->buf;
  } else {
    log_e("Camera capture failed");
    return;
  }

  // we're sending the initial header with the total number of chunks first
  // we can then later detect new frame with the header packets
  uint8_t totalChunks = (len + CHUNK_SIZE - 1) / CHUNK_SIZE;
  initial_packet_buffer[sizeof(ETVR_HEADER)] = totalChunks;
  socket.broadcastTo(initial_packet_buffer, sizeof(initial_packet_buffer),
                     this->STREAM_SERVER_PORT);

  for (uint8_t i = 0; i < totalChunks; i++) {
    auto offset = i * CHUNK_SIZE;
    // we need to make sure we don't overread
    auto chunkSize = (offset + CHUNK_SIZE <= len) ? CHUNK_SIZE : len - offset;
    packet_buffer[0] = static_cast<uint8_t>(i);
    // since this is a pointer, we can just add an offset to it, with a
    // chunksize to read and we're done
    memcpy(packet_buffer + 1, buf + offset, chunkSize);
    socket.broadcastTo(packet_buffer, chunkSize + 1, this->STREAM_SERVER_PORT);
  }

  if (fb) {
    esp_camera_fb_return(fb);
    fb = NULL;
    buf = NULL;
  } else if (buf) {
    free(buf);
    buf = NULL;
  }

  long request_end = millis();
  long latency = request_end - last_request_time;
  last_request_time = request_end;

  log_d("Size: %uKB, Time: %ums (%ifps) chunks: %u \n", len / 1024, latency,
        1000 / latency, totalChunks);
}

bool StreamServer::startTCPStreamServer() {
  tcp_server = new AsyncServer(this->STREAM_SERVER_PORT);
  tcp_server->onClient(
      [this](void* arg, AsyncClient* client) {
        this->handleNewTCPClient(arg, client);
      },
      tcp_server);

  tcp_server->begin();
  return true;
}

void StreamServer::handleNewTCPClient(void* arg, AsyncClient* client) {
  Serial.printf("Client connecting with ip: %s \n\r",
                client->remoteIP().toString().c_str());

  if (this->tcp_connected_client == nullptr) {
    this->tcp_connected_client = client;

    this->tcp_connected_client->onError(
        [this](void* arg, AsyncClient* client, int8_t error) {
          Serial.printf("\n connection error %s from client %s \n",
                        client->errorToString(error),
                        client->remoteIP().toString().c_str());

          this->tcp_connected_client = nullptr;
        });

    this->tcp_connected_client->onDisconnect(
        [this](void* arg, AsyncClient* client) {
          this->tcp_connected_client = nullptr;
        });

    Serial.println("Client connected!");
  } else {
    client->close();
    Serial.println("Rejected client, only one connection allowed!");
  }
}

void StreamServer::sendTCPFrame() {
  if (this->tcp_connected_client == nullptr ||
      !this->tcp_connected_client->connected()) {
    return;
  }

  if (!last_frame)
    last_frame = esp_timer_get_time();

  auto fb = esp_camera_fb_get();
  if (!fb) {
    log_e("Camera capture failed");
    return;
  }

  size_t len = fb->len;

  this->tcp_connected_client->write(ETVR_HEADER_BYTES, 4);
  this->tcp_connected_client->write((const char*)fb->buf, fb->len);

  if (fb) {
    esp_camera_fb_return(fb);
  }

  long request_end = millis();
  long latency = request_end - last_request_time;
  last_request_time = request_end;
  log_d("Size: %uKB, Time: %ums (%ifps)\n", len / 1024, latency,
        1000 / latency);
}