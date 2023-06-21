#pragma once
#include <Arduino.h>
#include <esp_camera.h>
#include <utilities/observer.hpp>
#include <utilities/network_utilities.hpp>
#include "local/data/config/config.hpp"
//#include "data/StateManager/StateManager.hpp"

class CameraHandler : public IObserver<ConfigState_e> {
 private:
  sensor_t* camera_sensor;
  camera_config_t config;
  OpenIrisConfig& configManager;

 public:
  CameraHandler(OpenIrisConfig& configManager);
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
