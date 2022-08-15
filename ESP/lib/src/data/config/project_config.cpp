#include "project_config.hpp"

Preferences preferences;

ProjectConfig::ProjectConfig() : Config(&preferences, "config"), _already_loaded(false) {}

ProjectConfig::~ProjectConfig() {}

/**
 *@brief Initializes the structures with blank data to prevent empty memory sectors and nullptr errors.
 *@brief This is to be called in setup() before loading the config.
 */
void ProjectConfig::initConfig()
{
    begin();
    this->config.device = {
        "EyeTrackVR",
        "",
        3232,
        false,
        false,
        false,
        "",
        "",
        ""
    };

    this->config.camera = {
        0,
        0,
        0,
        0,
    };

    this->config.networks = {
        {
            "",
            "",
            "",
            0,
        },
    };

    this->config.ap_network = {
        "",
        "",
        0,
    };
}

void ProjectConfig::load()
{
    log_d("Loading project config");
    if (this->_already_loaded)
    {
        log_w("Project config already loaded");
        return;
    }

    bool device_name_success = this->read("device_name", this->config.device.name);
    bool device_otapassword_success = this->read("ota_pass", this->config.device.OTAPassword);
    bool device_otaport_success = this->read("ota_port", this->config.device.OTAPort);

    bool device_success = device_name_success && device_otapassword_success && device_otaport_success;

    bool camera_vflip_success = this->read("camera_vflip", this->config.camera.vflip);
    bool camera_framesize_success = this->read("cameraFrmsz", this->config.camera.framesize);
    bool camera_href_success = this->read("camera_href", this->config.camera.href);
    bool camera_quality_success = this->read("camera_quality", this->config.camera.quality);

    bool camera_success = camera_vflip_success && camera_framesize_success && camera_href_success && camera_quality_success;

    bool network_info_success;
    for (int i = 0; i < this->config.networks.size(); i++)
    {
        char buff[25];
        snprintf(buff, sizeof(buff), "%d_name", i);
        bool networks_name_success = this->read(buff, this->config.networks[i].name);
        snprintf(buff, sizeof(buff), "%d_ssid", i);
        bool networks_ssid_success = this->read(buff, this->config.networks[i].ssid);
        snprintf(buff, sizeof(buff), "%d_password", i);
        bool networks_password_success = this->read(buff, this->config.networks[i].password);
        snprintf(buff, sizeof(buff), "%d_channel", i);
        bool networks_channel_success = this->read(buff, this->config.networks[i].channel);

        network_info_success = networks_name_success && networks_ssid_success && networks_password_success && networks_channel_success;
    }

    if (!device_success || !camera_success || !network_info_success)
    {
        log_e("Failed to load project config - Generating config and restarting");
        save();
        delay(1000);
        ESP.restart();
        return;
    }

    this->_already_loaded = true;
    this->notify(ObserverEvent::configLoaded);
}

void ProjectConfig::save()
{
    log_d("Saving project config");

    this->write("device_name", this->config.device.name);
    this->write("ota_pass", this->config.device.OTAPassword);
    this->write("ota_port", this->config.device.OTAPort);

    this->write("camera_vflip", this->config.camera.vflip);
    this->write("cameraFrmsz", this->config.camera.framesize);
    this->write("camera_href", this->config.camera.href);
    this->write("camera_quality", this->config.camera.quality);

    for (int i = 0; i < this->config.networks.size(); i++)
    {
        char buff[25];
        snprintf(buff, sizeof(buff), "%d_name", i);
        this->write(buff, this->config.networks[i].name);
        snprintf(buff, sizeof(buff), "%d_ssid", i);
        this->write(buff, this->config.networks[i].ssid);
        snprintf(buff, sizeof(buff), "%d_password", i);
        this->write(buff, this->config.networks[i].password);
        snprintf(buff, sizeof(buff), "%d_channel", i);
        this->write(buff, this->config.networks[i].channel);
    }
}

void ProjectConfig::reset()
{
    log_w("Resetting project config");
    this->clear();
}

//**********************************************************************************************************************
//*
//*                                                DeviceConfig
//*
//**********************************************************************************************************************
void ProjectConfig::setDeviceConfig(const char *name, const char *OTAPassword, int *OTAPort, bool shouldNotify)
{
    log_d("Updating device config");
    this->config.device = {
        (char *)name,
        (char *)OTAPassword,
        *OTAPort,
    };
    if (shouldNotify)
    {
        this->notify(ObserverEvent::deviceConfigUpdated);
    }
}

void ProjectConfig::setCameraConfig(uint8_t *vflip, uint8_t *framesize, uint8_t *href, uint8_t *quality, bool shouldNotify)
{
    this->config.camera = {
        *vflip,
        *framesize,
        *href,
        *quality,
    };

    log_d("Updating camera config");
    if (shouldNotify)
    {
        this->notify(ObserverEvent::cameraConfigUpdated);
    }
}

void ProjectConfig::setWifiConfig(const char *networkName, const char *ssid, const char *password, uint8_t *channel, bool shouldNotify)
{
    WiFiConfig_t *networkToUpdate = nullptr;

    for (int i = 0; i < this->config.networks.size(); i++)
    {
        if (strcmp(this->config.networks[i].name.c_str(), networkName) == 0)
            networkToUpdate = &this->config.networks[i];
    }

    if (networkToUpdate != nullptr)
    {
        this->config.networks = {
            {
                (char *)networkName,
                (char *)ssid,
                (char *)password,
                *channel,
            },
        };
        if (shouldNotify)
            this->notify(ObserverEvent::networksConfigUpdated);
    }
    log_d("Updating wifi config");
}

void ProjectConfig::setAPWifiConfig(const char *ssid, const char *password, uint8_t *channel, bool shouldNotify)
{
    this->config.ap_network = {
        (char *)ssid,
        (char *)password,
        *channel,
    };

    log_d("Updating access point config");
    if (shouldNotify)
    {
        this->notify(ObserverEvent::networksConfigUpdated);
    }
}