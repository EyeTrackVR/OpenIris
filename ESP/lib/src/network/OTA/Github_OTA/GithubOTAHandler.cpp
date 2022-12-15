#include "GithubOTAHandler.hpp"

GithubOTAHandler::GithubOTAHandler() {}

GithubOTAHandler::~GithubOTAHandler() {}

void GithubOTAHandler::githubOTAEventCallback(void *handler_args, esp_event_base_t base, int32_t id, void *event_data)
{
    Github_OTA_client_handle_t *client = (Github_OTA_client_handle_t *)handler_args;
    ESP_LOGI(TAG, "Got Update Callback: %s", Github_OTA_get_event_str((Github_OTA_event_e)id));

    if (id == Github_OTA_EVENT_FIRMWARE_UPDATE_PROGRESS)
    {
        /* display some progress with the firmware update */
        ESP_LOGI(TAG, "Firmware Update Progress: %d%%", *((int *)event_data));
    }
    (void)client;
    return;
}

void GithubOTAHandler::updateFirmware(const std::string &binaryName)
{
    ESP_LOGI(TAG, "Starting Github OTA Update Handler");

    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // initialize our Github_OTA config
    Github_OTA_config_t ghconfig = {
        .filenamematch = (char *)binaryName.c_str(),
        /* 1 minute as a example, but in production you should pick something larger (remember, Github has ratelimits on the API! )*/
        .updateInterval = 30,
    };
    // initialize Github_OTA
    Github_OTA_client_handle_t *Github_OTA_client = Github_OTA_init(&ghconfig);
    if (Github_OTA_client == NULL)
    {
        ESP_LOGE(TAG, "Github_OTA_client_init failed");
        return;
    }
    // register for events relating to the update progress
    esp_event_handler_register(Github_OTA_EVENTS, ESP_EVENT_ANY_ID, &githubOTAEventCallback, Github_OTA_client);

#if DO_BACKGROUND_UPDATE
    /* for private repositories or to get more API calls than anonymous, set a github username and PAT token
     * see https://docs.github.com/en/github/authenticating-to-github/creating-a-personal-access-token
     * for more information on how to create a PAT token.
     *
     * Be carefull, as the PAT token will be stored in your firmware etc and can be used to access your github account.
     */
    // ESP_ERROR_CHECK(Github_OTA_set_auth(Github_OTA_client, "<Insert GH Username>", "<insert PAT TOKEN>"));

    // start a timer that will automatically check for updates based on the interval specified above
    ESP_ERROR_CHECK(Github_OTA_start_update_timer(Github_OTA_client));

#elif DO_FORGROUND_UPDATE
    /* or do a check/update now
     * This runs in a new task under freeRTOS, so you can do other things while it is running.
     */
    ESP_ERROR_CHECK(Github_OTA_start_update_task(Github_OTA_client));

#elif DO_MANUAL_CHECK_UPDATE
    /* Alternatively you can do manual checks
     * but note, you probably have to increase the Stack size for the task this runs on
     */

    /* Query the Github Release API for the latest release */
    ESP_ERROR_CHECK(Github_OTA_check(Github_OTA_client));

    /* get the semver version of the currently running firmware */
    semver_t *cur = Github_OTA_get_current_version(Github_OTA_client);
    if (cur)
    {
        ESP_LOGI(TAG, "Current version: %d.%d.%d", cur->major, cur->minor, cur->patch);
        semver_free(cur);

        /* get the version of the latest release on Github */
        semver_t *new = Github_OTA_get_latest_version(Github_OTA_client);
        if (new)
        {
            ESP_LOGI(TAG, "New version: %d.%d.%d", new->major, new->minor, new->patch);
            semver_free(new);
        }

        /* do some comparisions */
        if (semver_gt(new, cur) == 1)
        {
            ESP_LOGI(TAG, "New version is greater than current version");
        }
        else if (semver_eq(new, cur) == 1)
        {
            ESP_LOGI(TAG, "New version is equal to current version");
        }
        else
        {
            ESP_LOGI(TAG, "New version is less than current version");
        }

        /* assuming we have a new version, then do a actual update */
        ESP_ERROR_CHECK(Github_OTA_update(Github_OTA_client));
        /* if there was a new version installed, the esp will reboot after installation and will not reach this code */

#endif
}