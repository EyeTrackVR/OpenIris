#include "stream_server.hpp"

// ***********************************************************************************
//!                             Global Variables
// ***********************************************************************************

constexpr static const char* STREAM_CONTENT_TYPE =
    "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
constexpr static const char* STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
/* constexpr static const char* STREAM_PART =
    "Content-Type: image/jpeg\r\nContent-Length: %u\r\nX-Timestamp: "
    "%d.%06d\r\n\r\n";
 */
constexpr static const char* STREAM_PART =
    "Content-Type: %s\r\nContent-Length: %u\r\n\r\n";
static const char* JPG_CONTENT_TYPE = "image/jpeg";

// ***********************************************************************************
//!                             AsyncBufferResponse
// ***********************************************************************************

AsyncBufferResponse::AsyncBufferResponse(uint8_t* buf,
                                         size_t len,
                                         const char* contentType)
    : _buf(buf), _len(len) {
  _callback = nullptr;
  _code = 200;
  _contentLength = _len;
  _contentType = contentType;
  _index = 0;
}

AsyncBufferResponse::~AsyncBufferResponse() {
  if (_buf != nullptr) {
    free(_buf);
  }
}

bool AsyncBufferResponse::_sourceValid() const {
  return _buf != nullptr;
}

size_t AsyncBufferResponse::_fillBuffer(uint8_t* buf, size_t maxLen) {
  size_t ret = _content(buf, maxLen, _index);
  if (ret != RESPONSE_TRY_AGAIN) {
    _index += ret;
  }
  return ret;
}

size_t AsyncBufferResponse::_content(uint8_t* buffer,
                                     size_t maxLen,
                                     size_t index) {
  memcpy(buffer, _buf + index, maxLen);
  if ((index + maxLen) == _len) {
    free(_buf);
    _buf = nullptr;
  }
  return maxLen;
}

// ***********************************************************************************
//!                             AsyncFrameResponse
// ***********************************************************************************

AsyncFrameResponse::AsyncFrameResponse(camera_fb_t* frame,
                                       const char* contentType)
    : fb(frame), _index(0) {
  _callback = nullptr;
  _code = 200;
  _contentLength = frame->len;
  _contentType = contentType;
}

AsyncFrameResponse::~AsyncFrameResponse() {
  if (fb != nullptr) {
    esp_camera_fb_return(fb);
  }
}

bool AsyncFrameResponse::_sourceValid() const {
  return fb != nullptr;
}

size_t AsyncFrameResponse::_content(uint8_t* buffer,
                                    size_t maxLen,
                                    size_t index) {
  memcpy(buffer, fb->buf + index, maxLen);
  if ((index + maxLen) == fb->len) {
    esp_camera_fb_return(fb);
    fb = nullptr;
  }
  return maxLen;
}

size_t AsyncFrameResponse::_fillBuffer(uint8_t* buf, size_t maxLen) {
  size_t ret = _content(buf, maxLen, _index);
  if (ret != RESPONSE_TRY_AGAIN) {
    _index += ret;
  }
  return ret;
}

// ***********************************************************************************
//!                             AsyncJpegStreamResponse
// ***********************************************************************************

AsyncJpegStreamResponse::AsyncJpegStreamResponse()
    : _index(0), _jpg_buf_len(0), _jpg_buf(NULL), lastAsyncRequest(0) {
  _callback = nullptr;
  _code = 200;
  _contentLength = 0;
  _contentType = STREAM_CONTENT_TYPE;
  _sendContentLength = false;
  _chunked = true;
  memset(&_frame, 0, sizeof(camera_frame_t));
}

AsyncJpegStreamResponse::~AsyncJpegStreamResponse() {
  if (_frame.fb) {
    if (_frame.fb->format != PIXFORMAT_JPEG) {
      free(_jpg_buf);
    }
    esp_camera_fb_return(_frame.fb);
  }
}

bool AsyncJpegStreamResponse::_sourceValid() const {
  return true;
}

size_t AsyncJpegStreamResponse::_fillBuffer(uint8_t* buf, size_t maxLen) {
  size_t ret = _content(buf, maxLen, _index);
  if (ret != RESPONSE_TRY_AGAIN) {
    _index += ret;
  }
  return ret;
}

size_t AsyncJpegStreamResponse::_content(uint8_t* buffer,
                                         size_t maxLen,
                                         size_t index) {
  if (!_frame.fb || _frame.index == _jpg_buf_len) {
    if (index && _frame.fb) {
      long end = millis();
      int fp = (end - lastAsyncRequest);
      log_d("[Stream Server]: Size: %uKB, Time: %ums (%ifps)\n",
            _jpg_buf_len / 1024, fp, 1000 / fp);
      lastAsyncRequest = end;
      if (_frame.fb->format != PIXFORMAT_JPEG) {
        free(_jpg_buf);
      }
      esp_camera_fb_return(_frame.fb);
      _frame.fb = NULL;
      _jpg_buf_len = 0;
      _jpg_buf = NULL;
    }
    if (maxLen < (strlen(STREAM_BOUNDARY) + strlen(STREAM_PART) +
                  strlen(JPG_CONTENT_TYPE) + 8)) {
      // log_w("Not enough space for headers");
      return RESPONSE_TRY_AGAIN;
    }
    // get frame
    _frame.index = 0;

    _frame.fb = esp_camera_fb_get();
    if (_frame.fb == NULL) {
      log_e("[Stream Server]: Camera frame failed");
      return 0;
    }

    if (_frame.fb->format != PIXFORMAT_JPEG) {
      unsigned long st = millis();
      bool jpeg_converted = frame2jpg(_frame.fb, 80, &_jpg_buf, &_jpg_buf_len);
      if (!jpeg_converted) {
        log_e("[Stream Server]: JPEG compression failed");
        esp_camera_fb_return(_frame.fb);
        _frame.fb = NULL;
        _jpg_buf_len = 0;
        _jpg_buf = NULL;
        return 0;
      }
      log_i("[Stream Server]: JPEG: %lums, %uB", millis() - st, _jpg_buf_len);
    } else {
      _jpg_buf_len = _frame.fb->len;
      _jpg_buf = _frame.fb->buf;
    }

    // send boundary
    size_t blen = 0;
    if (index) {
      blen = strlen(STREAM_BOUNDARY);
      memcpy(buffer, STREAM_BOUNDARY, blen);
      buffer += blen;
    }
    // send header
    size_t hlen =
        sprintf((char*)buffer, STREAM_PART, JPG_CONTENT_TYPE, _jpg_buf_len);
    buffer += hlen;
    // send frame
    hlen = maxLen - hlen - blen;
    if (hlen > _jpg_buf_len) {
      maxLen -= hlen - _jpg_buf_len;
      hlen = _jpg_buf_len;
    }
    memcpy(buffer, _jpg_buf, hlen);
    _frame.index += hlen;
    return maxLen;
  }

  size_t available = _jpg_buf_len - _frame.index;
  if (maxLen > available) {
    maxLen = available;
  }
  memcpy(buffer, _jpg_buf + _frame.index, maxLen);
  _frame.index += maxLen;

  return maxLen;
}

// ***********************************************************************************
//!                             Camera Server
// ***********************************************************************************

StreamServer::StreamServer(const int STREAM_PORT) : server(STREAM_PORT) {}

void StreamServer::begin() {
  server.on("/", HTTP_GET,
            [this](AsyncWebServerRequest* request) { this->stream(request); });
  server.begin();
}

void StreamServer::stream(AsyncWebServerRequest* request) {
  camera_fb_t* fb = esp_camera_fb_get();
  if (fb == NULL) {
    log_e("%s", F("Webrequest: \"/stream\" -> Camera not Detected.\n"));
    String s =
        F("Camera not Detected.<script>setTimeout(function() "
          "{window.parent.location.href= \"/\";s}, 3000);</script>");
    request->send(200, "text/html", s);
    return;
  }
  log_i("%s", F("Start JPG streaming\n"));
  AsyncJpegStreamResponse* response = new AsyncJpegStreamResponse();
  if (!response) {
    request->send(500);
    return;
  }
  response->addHeader("Access-Control-Allow-Origin", "*");
  response->addHeader("X-Framerate", "60");
  request->send(response);
}