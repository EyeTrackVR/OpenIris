#pragma once
#ifndef STREAM_SERVER_HPP
#define STREAM_SERVER_HPP
#define PART_BOUNDARY "123456789000000000000987654321"
#include <Arduino.h>
#include <AsyncTCP.h>
#include <AsyncUDP.h>
#include <WiFi.h>

#include <iomanip>
#include <sstream>

#include "data/StateManager/StateManager.hpp"
#include "data/utilities/helpers.hpp"

// Camera includes
#include "esp_camera.h"
#include "esp_http_server.h"
#include "esp_timer.h"
#include "fb_gfx.h"
#include "img_converters.h"

constexpr int CHUNK_SIZE = 1323;  // old: 1024;

namespace StreamHelpers {
  esp_err_t stream(httpd_req_t* req);
}
// namespace StreamHelpers
class StreamServer {
 private:
  AsyncUDP socket;
  AsyncServer* tcp_server;
  AsyncClient* tcp_connected_client;
  httpd_handle_t camera_stream = nullptr;
  uint8_t initial_packet_buffer[6];
  uint8_t packet_buffer[CHUNK_SIZE];

  int64_t last_frame = 0;
  long last_request_time = 0;

  int STREAM_SERVER_PORT;

 public:
  StreamServer(const int STREAM_PORT = 80);
  int startStreamServer();
  bool startUDPStreamServer();
  bool startTCPStreamServer();

  // rewrite this to an RTOS task pinned to the second core, for testing this is
  // fine https://randomnerdtutorials.com/esp32-dual-core-arduino-ide/
  void sendUDPFrame();

  void sendTCPFrame();
  void handleNewTCPClient(void* arg, AsyncClient* client);
};

#endif  // STREAM_SERVER_HPP
