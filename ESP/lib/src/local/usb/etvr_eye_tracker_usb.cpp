#include "etvr_eye_tracker_usb.hpp"

#include <Arduino.h>
#include <USBCDC.h>
#include <esp_camera.h>

const char* const ETVR_HEADER = "\xff\xa0";
const char* const ETVR_HEADER_FRAME = "\xff\xa1";

void usb_init() {
  Serial.begin(3000000);
  Serial.flush();
}

void usb_loop() {
  int64_t last_frame = 0;
  if (!last_frame)
    last_frame = esp_timer_get_time();

  long last_request_time = 0;
  camera_fb_t* fb = NULL;
  esp_err_t err = ESP_OK;

  size_t len = 0;
  uint8_t* buf = NULL;

  uint8_t len_bytes[2];

  while (true) {
    fb = esp_camera_fb_get();
    if (fb) {
      len = fb->len;
      buf = fb->buf;
    } else {
      log_e("Camera capture failed with response: %s", esp_err_to_name(err));
      err = ESP_FAIL;
    }
    if (err == ESP_OK)
      Serial.write(ETVR_HEADER, 2);
    Serial.write(ETVR_HEADER_FRAME, 2);
    len_bytes[0] = len & 0xFF;
    len_bytes[1] = (len >> CHAR_BIT) & 0xFF;
    Serial.write(len_bytes, 2);
    Serial.write((const char*)buf, len);
    if (fb) {
      esp_camera_fb_return(fb);
      fb = NULL;
      buf = NULL;
    } else if (buf) {
      free(buf);
      buf = NULL;
    }
    if (err != ESP_OK)
      break;
    long request_end = millis();
    long latency = request_end - last_request_time;
    last_request_time = request_end;
    log_d("Size: %uKB, Time: %ums (%ifps)\n", len / 1024, latency,
          1000 / latency);
  }
}
