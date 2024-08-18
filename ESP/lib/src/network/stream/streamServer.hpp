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
#include "data/config/project_config.hpp"
#include "data/utilities/Observer.hpp"
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
class StreamServer : public IObserver<ConfigState_e> {
 private:
  ProjectConfig& configManager;
  int64_t last_frame = 0;

  AsyncServer* tcp_server;
  AsyncClient* tcp_connected_client;
  httpd_handle_t camera_stream = nullptr;

  long last_request_time = 0;
  int STREAM_SERVER_PORT = 80;
  int TCP_STREAM_SERVER_PORT = 82;

  int last_time_frame_sent = 0;
  float target_fps_time = 1000 / 30;

  bool pauseTCPStream = true;

 public:
  StreamServer(ProjectConfig& configManager,
               const int STREAM_PORT,
               const int TPC_SERVER_PORT)
      : configManager(configManager),
        STREAM_SERVER_PORT(STREAM_PORT),
        TCP_STREAM_SERVER_PORT(TPC_SERVER_PORT) {};

  int startStreamServer();
  bool startTCPStreamServer();

  void toggleTCPStream(bool state);
  void sendTCPFrame();
  void handleNewTCPClient(void* arg, AsyncClient* client);

  void update(ConfigState_e event) override;
  std::string getName() override;
};

#endif  // STREAM_SERVER_HPP
