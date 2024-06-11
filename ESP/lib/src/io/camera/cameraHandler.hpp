#pragma once
#include <Arduino.h>
#include <esp_camera.h>
#include "data/StateManager/StateManager.hpp"
#include "data/config/project_config.hpp"
#include "data/utilities/Observer.hpp"
#include "data/utilities/network_utilities.hpp"

#define DEFAULT_XCLK_FREQ_HZ 16500000
#define USB_DEFAULT_XCLK_FREQ_HZ 24000000
#define OV5640_XCLK_FREQ_HZ DEFAULT_XCLK_FREQ_HZ

class CameraHandler : public IObserver<ConfigState_e> {
 private:
  sensor_t* camera_sensor;
  camera_config_t config;
  ProjectConfig& configManager;

 public:
  CameraHandler(ProjectConfig& configManager);
  int setCameraResolution(framesize_t frameSize);
  int setVFlip(int direction);
  int setHFlip(int direction);
  int setVieWindow(int offsetX, int offsetY, int outputX, int outputY);
  void update(ConfigState_e event);
  std::string getName();
  void resetCamera(bool type = 0);

 private:
  void loadConfigData();
  bool setupCamera();
  void setupCameraPinout();
  void setupBasicResolution();
  void setupCameraSensor();
};
