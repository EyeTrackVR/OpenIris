#pragma once
#ifndef STREAM_SERVER_HPP
#define STREAM_SERVER_HPP
#define PART_BOUNDARY "123456789000000000000987654321"
#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <WiFi.h>
#include <data/statemanager/state_manager.hpp>

// Camera includes
#include "esp_camera.h"
// #include "esp_http_server.h"
#include "esp_timer.h"
#include "fb_gfx.h"
#include "img_converters.h"

typedef struct {
  camera_fb_t* fb;
  size_t index;
} camera_frame_t;

class AsyncBufferResponse : public AsyncAbstractResponse {
 private:
  uint8_t* _buf;
  size_t _len;
  size_t _index;

 public:
  AsyncBufferResponse(uint8_t* buf, size_t len, const char* contentType);
  ~AsyncBufferResponse();
  bool _sourceValid() const;
  virtual size_t _fillBuffer(uint8_t* buf, size_t maxLen) override;
  size_t _content(uint8_t* buffer, size_t maxLen, size_t index);
};

class AsyncFrameResponse : public AsyncAbstractResponse {
 private:
  camera_fb_t* fb;
  size_t _index;

 public:
  AsyncFrameResponse(camera_fb_t* frame, const char* contentType);
  ~AsyncFrameResponse();
  bool _sourceValid() const;
  virtual size_t _fillBuffer(uint8_t* buf, size_t maxLen) override;
  size_t _content(uint8_t* buffer, size_t maxLen, size_t index);
};

class AsyncJpegStreamResponse : public AsyncAbstractResponse {
 private:
  camera_frame_t _frame;
  size_t _index;
  size_t _jpg_buf_len;
  uint8_t* _jpg_buf;
  long lastAsyncRequest;

 public:
  AsyncJpegStreamResponse();
  ~AsyncJpegStreamResponse();
  bool _sourceValid() const;
  virtual size_t _fillBuffer(uint8_t* buf, size_t maxLen) override;
  size_t _content(uint8_t* buffer, size_t maxLen, size_t index);
};

class StreamServer {
 private:
  int STREAM_SERVER_PORT;
  AsyncWebServer server;

 public:
  StreamServer(const int STREAM_PORT = 80);
  void begin();
  void stream(AsyncWebServerRequest* request);
};

#endif  // STREAM_SERVER_HPP
