#ifndef GITHUB_OTA_HANDLER_HPP
#define GITHUB_OTA_HANDLER_HPP
// #include <Arduino.h>
#include <esp_err.h>
#include <esp_log.h>
#include "nvs_flash.h"

// Custom Libraries
#include "lib/esp_ota.h"

//C++ Libraries
#include <string>

// ESP && FreeRTOS Libraries
#define DO_BACKGROUND_UPDATE 1
#define DO_FOREGROUND_UPDATE 0
#define DO_MANUAL_CHECK_UPDATE 0

class GithubOTAHandler
{
private:

public:
    GithubOTAHandler();
    ~GithubOTAHandler();
    void updateFirmware(const std::string &binaryName);
    static void githubOTAEventCallback(void *handler_args, esp_event_base_t base, int32_t id, void *event_data);
};

#endif // GITHUB_OTA_HANDLER_HPP
