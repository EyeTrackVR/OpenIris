#pragma once
#ifndef SERIAL_MANAGER
#define SERIAL_MANAGER

#include <Arduino.h>
#include <ArduinoJson.h>
#include <USBCDC.h>
#include <esp_camera.h>
#include "data/CommandManager/CommandManager.hpp"
#include "data/config/project_config.hpp"

const char* const ETVR_HEADER = "\xff\xa0";
const char* const ETVR_HEADER_FRAME = "\xff\xa1";

class SerialManager {
 private:
  esp_err_t err = ESP_OK;
  CommandManager* commandManager;

#ifdef ETVR_EYE_TRACKER_USB_API
  int64_t last_frame = 0;
  long last_request_time = 0;

  void send_frame();
#endif

 public:
  SerialManager(CommandManager* commandManager);
  void init();
  void run();
};

#endif
