#pragma once
#include <local/data/config/config.hpp>
#include <local/data/statemanager/state_manager.hpp>
#include <local/io/camera/camera_handler.hpp>

class API {

 public:
  API(ProjectConfig& projectConfig, OpenIrisConfig& configManager);
  virtual ~API();

  void setCamera(uint8_t vflip,
                 uint8_t framesize,
                 uint8_t href,
                 uint8_t quality,
                 uint8_t brightness,
                 bool shouldNotify);
  void restartCamera(const char* mode);

  OpenIrisConfig& configManager;
  ProjectConfig& projectConfig;
  CameraHandler cameraHandler;
};
