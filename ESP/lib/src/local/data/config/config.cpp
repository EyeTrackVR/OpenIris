#include "config.hpp"

OpenIrisConfig::OpenIrisConfig(ProjectConfig& projectConfig)
    : projectConfig(projectConfig) {
  initConfig();
}

OpenIrisConfig::~OpenIrisConfig() {}

/**
 *@brief Initializes the structures with blank data to prevent empty memory
 *sectors and nullptr errors.
 */
void OpenIrisConfig::initConfig() {
  this->config.camera = {
      .vflip = 0,
      .href = 0,
      .framesize = 4,
      .quality = 7,
      .brightness = 2,
  };
}

void OpenIrisConfig::save() {
  cameraConfigSave();
  OpenIrisTasks::ScheduleRestart(2000);
}

void OpenIrisConfig::cameraConfigSave() {
  projectConfig.putInt("vflip", this->config.camera.vflip);
  projectConfig.putInt("href", this->config.camera.href);
  projectConfig.putInt("framesize", this->config.camera.framesize);
  projectConfig.putInt("quality", this->config.camera.quality);
  projectConfig.putInt("brightness", this->config.camera.brightness);
}

void OpenIrisConfig::load() {
  cameraConfigLoad();
}

void OpenIrisConfig::cameraConfigLoad() {
  this->config.camera.vflip = projectConfig.getInt("vflip", 0);
  this->config.camera.href = projectConfig.getInt("href", 0);
  this->config.camera.framesize = projectConfig.getInt("framesize", 4);
  this->config.camera.quality = projectConfig.getInt("quality", 7);
  this->config.camera.brightness = projectConfig.getInt("brightness", 2);

  this->notifyAll(ConfigState_e::cameraConfigLoad);
}

void OpenIrisConfig::setCameraConfig(uint8_t vflip,
                                     uint8_t framesize,
                                     uint8_t href,
                                     uint8_t quality,
                                     uint8_t brightness,
                                     bool shouldNotify) {
  log_d("Updating camera config");
  this->config.camera.vflip = vflip;
  this->config.camera.href = href;
  this->config.camera.framesize = framesize;
  this->config.camera.quality = quality;
  this->config.camera.brightness = brightness;

  log_d("Updating Camera config");

  if (shouldNotify)
    this->notifyAll(ConfigState_e::cameraConfigUpdate);
}

std::string OpenIrisConfig::CameraConfig_t::toRepresentation() {
  std::string json = Helpers::format_string(
      "\"camera_config\": {\"vflip\": %d,\"framesize\": %d,\"href\": "
      "%d,\"quality\": %d,\"brightness\": %d}",
      this->vflip, this->framesize, this->href, this->quality,
      this->brightness);
  return json;
}

//**********************************************************************************************************************
//*
//!                                                Get Methods
//*
//**********************************************************************************************************************
OpenIrisConfig::CameraConfig_t& OpenIrisConfig::getCameraConfig() {
  return this->config.camera;
}
