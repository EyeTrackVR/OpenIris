#ifndef GITHUB_OTA_H
#define GITHUB_OTA_H

#include <stdlib.h>
#include <fnmatch.h>
#include <libgen.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>
#include <esp_http_client.h>
#include <esp_tls.h>
#include <esp_crt_bundle.h>
#include <esp_log.h>
#include <esp_app_format.h>
#include <esp_ota_ops.h>
#include <esp_https_ota.h>
#include <esp_event.h>
#include <sdkconfig.h>
#include "LWJSON/lwjson.h"

#include <esp_err.h>
#include <esp_event.h>
#include "SemVer/semver.h"

#ifdef __cplusplus
extern "C"
{
#endif

    static const char *TAG = "Github_OTA";
    ESP_EVENT_DECLARE_BASE(Github_OTA_EVENTS);

    /**
     * @brief Github OTA events
     * These events are posted to the event loop to track progress of the OTA process
     */
    typedef enum
    {
        Github_OTA_EVENT_START_CHECK = 0x01,               /*!< Github OTA check started */
        Github_OTA_EVENT_UPDATE_AVAILABLE = 0x02,          /*!< Github OTA update available */
        Github_OTA_EVENT_NOUPDATE_AVAILABLE = 0x04,        /*!< Github OTA no update available */
        Github_OTA_EVENT_START_UPDATE = 0x08,              /*!< Github OTA update started */
        Github_OTA_EVENT_FINISH_UPDATE = 0x10,             /*!< Github OTA update finished */
        Github_OTA_EVENT_UPDATE_FAILED = 0x20,             /*!< Github OTA update failed */
        Github_OTA_EVENT_START_STORAGE_UPDATE = 0x40,      /*!< Github OTA storage update started. If the storage is mounted, you should unmount it when getting this call */
        Github_OTA_EVENT_FINISH_STORAGE_UPDATE = 0x80,     /*!< Github OTA storage update finished. You can mount the new storage after getting this call if needed */
        Github_OTA_EVENT_STORAGE_UPDATE_FAILED = 0x100,    /*!< Github OTA storage update failed */
        Github_OTA_EVENT_FIRMWARE_UPDATE_PROGRESS = 0x200, /*!< Github OTA firmware update progress */
        Github_OTA_EVENT_STORAGE_UPDATE_PROGRESS = 0x400,  /*!< Github OTA storage update progress */
        Github_OTA_EVENT_PENDING_REBOOT = 0x800,           /*!< Github OTA pending reboot */
    } Github_OTA_event_e;

    /**
     * @brief Github OTA Configuration
     */
    typedef struct Github_OTA_config_t
    {
        char *filenamematch;    /*!< Filename to match against on Github indicating this is a firmware file */
        char storagenamematch[CONFIG_MAX_FILENAME_LEN]; /*!< Filename to match against on Github indicating this is a storage file */
        char storagepartitionname[17];                  /*!< Name of the storage partition to update */
        char *hostname;                                 /*!< Hostname of the Github server. Defaults to api.github.com*/
        char *orgname;                                  /*!< Name of the Github organization */
        char *reponame;                                 /*!< Name of the Github repository */
        uint32_t updateInterval;                        /*!< Interval in Minutes to check for updates if using the Github_OTA_start_update_timer function */
    } Github_OTA_config_t;

    typedef struct Github_OTA_client_handle_t Github_OTA_client_handle_t;

    /**
     * @brief  Initialize the github ota client
     *
     *
     * @param config [in] Configuration for the github ota client
     * @return Github_OTA_client_handle_t* handle to pass to all subsequent calls. If it returns NULL, there is a error in your config
     */
    Github_OTA_client_handle_t *Github_OTA_init(Github_OTA_config_t *config);

    /**
     * @brief Set the Username and Password to access private repositories or get more API calls
     *
     * Anonymus API calls are limited to 60 per hour. If you want to get more calls, you need to set a username and password.
     * Be aware that this will be stored in the flash and can be read by anyone with access to the flash.
     * The password should be a Github Personal Access Token and for good security you should limit what it can do
     *
     * @param handle the handle returned by Github_OTA_init
     * @param username the username to authenticate with
     * @param password this Github Personal Access Token
     * @return esp_err_t ESP_OK if all is good, ESP_FAIL if there is an error
     */
    esp_err_t Github_OTA_set_auth(Github_OTA_client_handle_t *handle, const char *username, const char *password);
    /**
     * @brief Free the Github_OTA client handle and all resources
     *
     * @param handle the Handle
     * @return esp_err_t if there was a error
     */
    esp_err_t Github_OTA_free(Github_OTA_client_handle_t *handle);

    /**
     * @brief Perform a check for updates
     *
     * This will just check if there is a available update on Github releases with download resources that match your configuration
     * for firmware and storage files. If it returns ESP_OK, you can call Github_OTA_get_latest_version to get the version of the latest release
     *
     * @param handle the Github_OTA_client_handle_t handle
     * @return esp_err_t ESP_OK if there is a update available, ESP_FAIL if there is no update available or an error
     */
    esp_err_t Github_OTA_check(Github_OTA_client_handle_t *handle);

    /**
     * @brief Downloads and writes the latest firmware and storage partition (if available)
     *
     * You should only call this after calling Github_OTA_check and ensuring that there is a update available.
     *
     * @param handle the Github_OTA_client_handle_t handle
     * @return esp_err_t ESP_FAIL if there is a error. If the Update is successful, it will not return, but reboot the device
     */
    esp_err_t Github_OTA_update(Github_OTA_client_handle_t *handle);

    /**
     * @brief Get the currently running version of the firmware
     *
     * This will return the version of the firmware currently running on your device.
     * consult semver.h for functions to compare versions
     *
     * @param handle the Github_OTA_client_handle_t handle
     * @return semver_t the version of the latest release
     */

    semver_t *Github_OTA_get_current_version(Github_OTA_client_handle_t *handle);

    /**
     * @brief Get the version of the latest release on Github. Only valid after calling Github_OTA_check
     *
     * @param handle the Github_OTA_client_handle_t handle
     * @return semver_t* the version of the latest release on Github
     */
    semver_t *Github_OTA_get_latest_version(Github_OTA_client_handle_t *handle);

    /**
     * @brief Start a new Task that will check for updates and update if available
     *
     * This is equivalent to calling Github_OTA_check and Github_OTA_update if there is a new update available.
     * If no update is available, it will not update the device.
     *
     * Progress can be monitored by registering for the Github_OTA_EVENTS events on the Global Event Loop
     *
     * @param handle Github_OTA_client_handle_t handle
     * @return esp_err_t ESP_OK if the task was started, ESP_FAIL if there was an error
     */
    esp_err_t Github_OTA_start_update_task(Github_OTA_client_handle_t *handle);

    /**
     * @brief Install a Timer to automatically check for new updates and update if available
     *
     * Install a timer that will check for new updates every updateInterval seconds and update if available.
     *
     * @param handle Github_OTA_client_handle_t handle
     * @return esp_err_t ESP_OK if no error, otherwise ESP_FAIL
     */

    esp_err_t Github_OTA_start_update_timer(Github_OTA_client_handle_t *handle);

    /**
     * @brief convience function to return a string representation of events emited by this library
     *
     * @param event the eventid passed to the event handler
     * @return char* a string representing the event
     */
    const char *Github_OTA_get_event_str(Github_OTA_event_e event);

#ifdef __cplusplus
}
#endif

#endif