#ifndef GITHUB_OTA_HANDLER_HPP
#define GITHUB_OTA_HANDLER_HPP
//#include <Arduino.h>

// ESP && FreeRTOS Libraries
#include <stdlib.h>
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include <lwip/sys.h>
#include <esp_err.h>
#include <esp_log.h>

// Custom Libraries
#include "lib/esp_ota.h"

#define DO_BACKGROUND_UPDATE 1
#define DO_FOREGROUND_UPDATE 0
#define DO_MANUAL_CHECK_UPDATE 0

class GithubOTAHandler
{
private:
    /* data */
public:
    GithubOTAHandler(/* args */);
    ~GithubOTAHandler();
    void updateFirmware();
    static void githubOTAEventCallback(void *handler_args, esp_event_base_t base, int32_t id, void *event_data);
};

#endif // GITHUB_OTA_HANDLER_HPP
