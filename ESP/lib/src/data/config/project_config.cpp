#include "project_config.hpp"

ProjectConfig::ProjectConfig() : Config("config", "nvs"), _already_loaded(false) {}

ProjectConfig::~ProjectConfig() {}

/**
 *@brief Initializes the structures with blank data to prevent empty memory sectors and nullptr errors.
 *@brief This is to be called in setup() before loading the config.
 */
void ProjectConfig::initStructures()
{
    this->config.device = {
        "",
        "",
        0,
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
        },
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

    bool device_success = this->read("device", this->config.device);
    bool camera_success = this->read("camera", this->config.camera);
    bool network_info_success = this->read("network_info", this->config.networks);
    
    if (!device_success || !camera_success || !network_info_success)
    {
        log_e("Failed to load project config");
        return;
    }

    this->_already_loaded = true;
    this->notify(ObserverEvent::configLoaded);
}

void ProjectConfig::save()
{
    log_d("Saving project config");
    this->write("device", this->config.device);
    this->write("camera", this->config.camera);
    this->write("network_info", this->config.networks);
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
        name,
        OTAPassword,
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

void ProjectConfig::setWifiConfig(const char *networkName, const char *ssid, const char *password, bool shouldNotify)
{
    WiFiConfig_t *networkToUpdate = nullptr;

    for (int i = 0; i < this->config.networks.size(); i++)
    {
        if (strcmp(this->config.networks[i].name, networkName) == 0)
            networkToUpdate = &this->config.networks[i];
    }

    if (networkToUpdate != nullptr)
    {
        this->config.networks = {
            {
                networkName,
                ssid,
                password,
            },
        };
        if (shouldNotify)
            this->notify(ObserverEvent::networksConfigUpdated);
    }
    log_d("Updating wifi config");
}

ProjectConfig projectConfig;