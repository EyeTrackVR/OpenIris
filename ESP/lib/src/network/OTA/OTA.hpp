#ifndef OTA_HPP
#define OTA_HPP
#include <ArduinoOTA.h>
#include <HTTPClient.h>
#include "data/config/project_config.hpp"

class OTA {
 public:
  OTA(ProjectConfig& _deviceConfig);
  virtual ~OTA();
  void begin();
  void handleOTAUpdate();

 private:
  ProjectConfig& _deviceConfig;
  unsigned long _bootTimestamp;
  bool _isOtaEnabled;
};
#endif  // OTA_HPP
