#include "OTA.hpp"
#include <AsyncElegantOTA.h>

OTA::OTA(ProjectConfig *_deviceConfig) : _deviceConfig(_deviceConfig){} 

OTA::~OTA() {}

void OTA::setup(AsyncWebServer* APIServer) {
  auto device_config = _deviceConfig->getDeviceConfig();

  if (device_config->OTAPassword.empty()){
    log_e("Password is empty, you need to provide a password in order to setup the OTA server");
    return;
  }

  AsyncElegantOTA.begin(APIServer, device_config->OTALogin.c_str(), device_config->OTAPassword.c_str());
}