#ifndef OTA_HPP
#define OTA_HPP
#include <ArduinoOTA.h>
#include <HTTPClient.h>
#include "data/Config/project_config.hpp"

class OTA
{
public:
    OTA(ProjectConfig *_deviceConfig);
    virtual ~OTA();

    void SetupOTA();

    void HandleOTAUpdate();

private:
    unsigned long _bootTimestamp = 0;
    bool _isOtaEnabled = true;
    ProjectConfig *_deviceConfig;
};
#endif // OTA_HPP