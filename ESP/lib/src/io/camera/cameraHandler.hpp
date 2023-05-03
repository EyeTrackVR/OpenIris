#pragma once
#include <Arduino.h>
#include <esp_camera.h>
#include "data/utilities/Observer.hpp"
#include "data/utilities/network_utilities.hpp"
#include "data/config/project_config.hpp"
#include "data/StateManager/StateManager.hpp"

class CameraHandler : public IObserver<ConfigState_e> {
 private:
  sensor_t* camera_sensor;
  camera_config_t config;
  ProjectConfig& configManager;

 public:
  CameraHandler(ProjectConfig& configManager);
  int setCameraResolution(framesize_t frameSize);
  void update(ConfigState_e event) override;
  std::string getName() override;
  void resetCamera(bool type = 0);

 private:
  void loadChangeableConfig();
  bool setupCamera();
  void setupCameraPinout();
  void setupCameraMemoryStorage();
  void setupCameraSensor();
};
