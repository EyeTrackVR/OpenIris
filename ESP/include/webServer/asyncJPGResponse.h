#pragma once
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "esp_camera.h"

#define PART_BOUNDARY "123456789000000000000987654321"

namespace OpenIris
{
  namespace WebServerHelpers
  {
    static const char *STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
    static const char *STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
    static const char *STREAM_PART = "Content-Type: %s\r\nContent-Length: %u\r\n\r\n";

    static const char *JPG_CONTENT_TYPE = "image/jpeg";
  }

  typedef struct
  {
    camera_fb_t *fb;
    size_t index;
  } camera_frame_t;

  class AsyncJpegStreamResponse : public AsyncAbstractResponse
  {
  private:
    camera_frame_t _frame;
    size_t _index;
    size_t _jpg_buf_len;
    uint8_t *_jpg_buf;
    long lastAsyncRequest;

  public:
    AsyncJpegStreamResponse()
    {
      _callback = nullptr;
      _code = 200;
      _contentLength = 0;
      _contentType = OpenIris::WebServerHelpers::STREAM_CONTENT_TYPE;
      _sendContentLength = false;
      _chunked = true;
      _index = 0;
      _jpg_buf_len = 0;
      _jpg_buf = NULL;
      lastAsyncRequest = 0;
      memset(&_frame, 0, sizeof(camera_frame_t));
    }
    ~AsyncJpegStreamResponse()
    {
      if (_frame.fb)
      {
        if (_frame.fb->format != PIXFORMAT_JPEG)
        {
          free(_jpg_buf);
        }
        esp_camera_fb_return(_frame.fb);
      }
    }
    bool _sourceValid() const
    {
      return true;
    }
    virtual size_t _fillBuffer(uint8_t *buf, size_t maxLen) override
    {
      size_t ret = _content(buf, maxLen, _index);
      if (ret != RESPONSE_TRY_AGAIN)
      {
        _index += ret;
      }
      return ret;
    }

    size_t _content(uint8_t *buffer, size_t maxLen, size_t index)
    {
      if (!_frame.fb || _frame.index == _jpg_buf_len)
      {
        if (index && _frame.fb)
        {
          long end = millis();
          int fp = (end - lastAsyncRequest);
          log_d("Size: %uKB, Time: %ums (%ifps)\n", _jpg_buf_len / 1024, fp, 1000 / fp);
          lastAsyncRequest = end;
          if (_frame.fb->format != PIXFORMAT_JPEG)
          {
            free(_jpg_buf);
          }
          esp_camera_fb_return(_frame.fb);
          _frame.fb = NULL;
          _jpg_buf_len = 0;
          _jpg_buf = NULL;
        }
        if (maxLen < (strlen(OpenIris::WebServerHelpers::STREAM_BOUNDARY) + strlen(OpenIris::WebServerHelpers::STREAM_PART) + strlen(OpenIris::WebServerHelpers::JPG_CONTENT_TYPE) + 8))
        {
          // log_w("Not enough space for headers");
          return RESPONSE_TRY_AGAIN;
        }
        // get frame
        _frame.index = 0;

        _frame.fb = esp_camera_fb_get();
        if (_frame.fb == NULL)
        {
          log_e("Camera frame failed");
          return 0;
        }

        if (_frame.fb->format != PIXFORMAT_JPEG)
        {
          unsigned long st = millis();
          bool jpeg_converted = frame2jpg(_frame.fb, 80, &_jpg_buf, &_jpg_buf_len);
          if (!jpeg_converted)
          {
            log_e("JPEG compression failed");
            esp_camera_fb_return(_frame.fb);
            _frame.fb = NULL;
            _jpg_buf_len = 0;
            _jpg_buf = NULL;
            return 0;
          }
          log_i("JPEG: %lums, %uB", millis() - st, _jpg_buf_len);
        }
        else
        {
          _jpg_buf_len = _frame.fb->len;
          _jpg_buf = _frame.fb->buf;
        }

        // send boundary
        size_t blen = 0;
        if (index)
        {
          blen = strlen(OpenIris::WebServerHelpers::STREAM_BOUNDARY);
          memcpy(buffer, OpenIris::WebServerHelpers::STREAM_BOUNDARY, blen);
          buffer += blen;
        }
        // send header
        size_t hlen = sprintf((char *)buffer, OpenIris::WebServerHelpers::STREAM_PART, OpenIris::WebServerHelpers::JPG_CONTENT_TYPE, _jpg_buf_len);
        buffer += hlen;
        // send frame
        hlen = maxLen - hlen - blen;
        if (hlen > _jpg_buf_len)
        {
          maxLen -= hlen - _jpg_buf_len;
          hlen = _jpg_buf_len;
        }
        memcpy(buffer, _jpg_buf, hlen);
        _frame.index += hlen;
        return maxLen;
      }

      size_t available = _jpg_buf_len - _frame.index;
      if (maxLen > available)
      {
        maxLen = available;
      }
      memcpy(buffer, _jpg_buf + _frame.index, maxLen);
      _frame.index += maxLen;

      return maxLen;
    }
  };
}
