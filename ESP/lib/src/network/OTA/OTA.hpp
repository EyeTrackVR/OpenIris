#ifndef OTA_HPP
#define OTA_HPP
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>

#include "data/config/project_config.hpp"

class OTA
{
public:
    OTA(ProjectConfig *_deviceConfig);
    virtual ~OTA();
    void setup(AsyncWebServer* APIServer);
private:
    ProjectConfig *_deviceConfig;
};
#endif // OTA_HPP
