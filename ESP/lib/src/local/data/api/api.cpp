#include "api.hpp"

//*********************************************************************************************
//!                                     Camera Command Functions
//*********************************************************************************************

API::API(ProjectConfig& projectConfig, OpenIrisConfig& configManager)
    : configManager(configManager),
      projectConfig(projectConfig),
      cameraHandler(configManager) {}

API::~API() {}

void API::setCamera(uint8_t vflip,
                    uint8_t framesize,
                    uint8_t href,
                    uint8_t quality,
                    uint8_t brightness,
                    bool shouldNotify) {
  configManager.setCameraConfig(vflip, framesize, href, quality, brightness,
                                true);
}

void API::restartCamera(const char* mode) {
  bool _mode = (bool)atoi(mode);
  cameraHandler.resetCamera(_mode);
}
