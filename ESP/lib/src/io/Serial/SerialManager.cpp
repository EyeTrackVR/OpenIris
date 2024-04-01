#include "SerialManager.hpp"

SerialManager::SerialManager(CommandManager* commandManager)
    : commandManager(commandManager) {}

#ifdef ETVR_EYE_TRACKER_USB_API
void SerialManager::send_frame() {
  // if we failed to capture the frame, we bail, but we still want to listen to commands
  if (err != ESP_OK)
    return;

  if (!last_frame)
    last_frame = esp_timer_get_time();

  uint8_t len_bytes[2];

  size_t len = 0;
  uint8_t* buf = NULL;

  auto fb = esp_camera_fb_get();
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

  long request_end = millis();
  long latency = request_end - last_request_time;
  last_request_time = request_end;
  log_d("Size: %uKB, Time: %ums (%ifps)\n", len / 1024, latency,
        1000 / latency);
}
#endif

void SerialManager::init() {
  Serial.begin(3000000);
  if (SERIAL_FLUSH_ENABLED){
    Serial.flush();
  }
}

void SerialManager::run() {
    if (Serial.available()) {
      JsonDocument doc;
      DeserializationError deserializationError = deserializeJson(doc, Serial);

      if (deserializationError) {
        log_e("Command deserialization failed: %s",
              deserializationError.c_str());
        
        return;
      }

      CommandsPayload commands = {doc};
      this->commandManager->handleCommands(commands);
    }
#ifdef ETVR_EYE_TRACKER_USB_API
    else {
      this->send_frame();
    }
#endif
}
