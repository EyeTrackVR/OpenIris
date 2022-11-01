#ifndef OTA_HPP
#define OTA_HPP
#include <ArduinoOTA.h>
#include <HTTPClient.h>
#include "data/config/project_config.hpp"

class OTA
{
public:
    OTA(ProjectConfig *_deviceConfig,
        const std::string &hostname);
    virtual ~OTA();
    void SetupOTA();
    void HandleOTAUpdate();

private:
    unsigned long _bootTimestamp = 0;
    bool _isOtaEnabled = true;
    ProjectConfig *_deviceConfig;
    std::string _hostname;
};
#endif // OTA_HPP