#include "project_config.hpp"

ProjectConfig::ProjectConfig(const std::string &name) : _name(std::move(name)), _already_loaded(false) {}

ProjectConfig::~ProjectConfig() {}

/**
 *@brief Initializes the structures with blank data to prevent empty memory sectors and nullptr errors.
 *@brief This is to be called in setup() before loading the config.
 */
void ProjectConfig::initConfig()
{
    if (_name.empty())
    {
        log_e("Config name is null\n");
        _name = "openiris";
    }

    bool success = begin(_name.c_str());

    log_i("Config name: %s", _name.c_str());
    log_i("Config loaded: %s", success ? "true" : "false");

    /*
    * If the config is not loaded,
    * we need to initialize the config with default data
    ! Do not initialize the WiFiConfig_t struct here,
    ! as it will create a blank network which breaks the WiFiManager
     */
    this->config.device = {
        "12345678",
        3232,
    };

    this->config.mdns = {
        "openiristracker",
        "",
    };

    this->config.ap_network = {
        "",
        "",
        1,
        false,
    };

    this->config.camera = {
        .vflip = 0,
        .href = 0,
        .framesize = 4,
        .quality = 7,
        .brightness = 2,
    };
}

void ProjectConfig::save()
{
    log_d("Saving project config");
    deviceConfigSave();
    mdnsConfigSave();
    cameraConfigSave();
    wifiConfigSave();
    end(); // we call end() here to close the connection to the NVS partition, we only do this because we call ESP.restart() next.
    ESP.restart();
}

void ProjectConfig::wifiConfigSave()
{
    log_d("Saving wifi config");

    /* WiFi Config */
    putInt("networkCount", this->config.networks.size());

    std::string name = "name";
    std::string ssid = "ssid";
    std::string password = "pass";
    std::string channel = "channel";
    for (int i = 0; i < this->config.networks.size(); i++)
    {
        char buffer[2];
        std::string iter_str = Helpers::itoa(i, buffer, 10);

        name.append(iter_str);
        ssid.append(iter_str);
        password.append(iter_str);
        channel.append(iter_str);

        putString(name.c_str(), this->config.networks[i].name.c_str());
        putString(ssid.c_str(), this->config.networks[i].ssid.c_str());
        putString(password.c_str(), this->config.networks[i].password.c_str());
        putInt(channel.c_str(), this->config.networks[i].channel);

        name = "name";
        ssid = "ssid";
        password = "pass";
        channel = "channel";
    }

    /* AP Config */
    putString("apSSID", this->config.ap_network.ssid.c_str());
    putString("apPass", this->config.ap_network.password.c_str());
    putUInt("apChannel", this->config.ap_network.channel);

    log_i("Project config saved and system is rebooting");
}

void ProjectConfig::deviceConfigSave()
{
    /* Device Config */
    putString("OTAPassword", this->config.device.OTAPassword.c_str());
    putInt("OTAPort", this->config.device.OTAPort);
}

void ProjectConfig::mdnsConfigSave()
{
    /* Device Config */
    putString("hostname", this->config.mdns.hostname.c_str());
    putString("service", this->config.mdns.service.c_str());
}

void ProjectConfig::cameraConfigSave()
{
    /* Camera Config */
    putInt("vflip", this->config.camera.vflip);
    putInt("href", this->config.camera.href);
    putInt("framesize", this->config.camera.framesize);
    putInt("quality", this->config.camera.quality);
    putInt("brightness", this->config.camera.brightness);
}

bool ProjectConfig::reset()
{
    log_w("Resetting project config");
    return clear();
}

void ProjectConfig::load()
{
    log_d("Loading project config");
    if (this->_already_loaded)
    {
        log_w("Project config already loaded");
        return;
    }

    /* Device Config */
    this->config.device.OTAPassword = getString("OTAPassword", "12345678").c_str();
    this->config.device.OTAPort = getInt("OTAPort", 3232);

    /* MDNS Config */
    this->config.mdns.hostname = getString("hostname").c_str();
    this->config.mdns.service = getString("service").c_str();
    /* WiFi Config */
    int networkCount = getInt("networkCount", 0);
    std::string name = "name";
    std::string ssid = "ssid";
    std::string password = "pass";
    std::string channel = "channel";
    for (int i = 0; i < networkCount; i++)
    {
        char buffer[2];
        std::string iter_str = Helpers::itoa(i, buffer, 10);

        name.append(iter_str);
        ssid.append(iter_str);
        password.append(iter_str);
        channel.append(iter_str);

        const std::string &temp_1 = getString(name.c_str()).c_str();
        const std::string &temp_2 = getString(ssid.c_str()).c_str();
        const std::string &temp_3 = getString(password.c_str()).c_str();
        uint8_t temp_4 = getUInt(channel.c_str());

        name = "name";
        ssid = "ssid";
        password = "pass";
        channel = "channel";

        //! push_back creates a copy of the object, so we need to use emplace_back
        this->config.networks.emplace_back(
            temp_1,
            temp_2,
            temp_3,
            temp_4,
            false); // false because the networks we store in the config are the ones we want the esp to connect to, rather than host as AP
    }

    /* AP Config */
    this->config.ap_network.ssid = getString("apSSID", "openiris").c_str();
    this->config.ap_network.password = getString("apPass", "12345678").c_str();
    this->config.ap_network.channel = getUInt("apChannel", 1);

    /* Camera Config */
    this->config.camera.vflip = getInt("vflip", 0);
    this->config.camera.href = getInt("href", 0);
    this->config.camera.framesize = getInt("framesize", 4);
    this->config.camera.quality = getInt("quality", 7);
    this->config.camera.brightness = getInt("brightness", 2);

    this->_already_loaded = true;
    this->notify(ObserverEvent::configLoaded);
}

//**********************************************************************************************************************
//*
//!                                                DeviceConfig
//*
//**********************************************************************************************************************
void ProjectConfig::setDeviceConfig(const std::string &OTAPassword, int *OTAPort, bool shouldNotify)
{
    log_d("Updating device config");
    this->config.device.OTAPassword.assign(OTAPassword);
    this->config.device.OTAPort = *OTAPort;

    if (shouldNotify)
        this->notify(ObserverEvent::deviceConfigUpdated);
}

void ProjectConfig::setMDNSConfig(const std::string &hostname, const std::string &service, bool shouldNotify)
{
    log_d("Updating MDNS config");
    this->config.mdns.hostname.assign(hostname);
    this->config.mdns.service.assign(service);

    if (shouldNotify)
        this->notify(ObserverEvent::mdnsConfigUpdated);
}

void ProjectConfig::setCameraConfig(uint8_t *vflip, uint8_t *framesize, uint8_t *href, uint8_t *quality, uint8_t *brightness, bool shouldNotify)
{
    log_d("Updating camera config");
    this->config.camera.vflip = *vflip;
    this->config.camera.href = *href;
    this->config.camera.framesize = *framesize;
    this->config.camera.quality = *quality;
    this->config.camera.brightness = *brightness;

    log_d("Updating Camera config");
    if (shouldNotify)
        this->notify(ObserverEvent::cameraConfigUpdated);
}

void ProjectConfig::setWifiConfig(const std::string &networkName, const std::string &ssid, const std::string &password, uint8_t *channel, bool adhoc, bool shouldNotify)
{
    WiFiConfig_t *networkToUpdate = nullptr;

    // we store the ADHOC flag as false because the networks we store in the config are the ones we want the esp to connect to, rather than host as AP, and here we're just updating them
    size_t size = this->config.networks.size();
    if (size > 0)
    {
        for (int i = 0; i < size; i++)
        {
            if (strcmp(this->config.networks[i].name.c_str(), networkName.c_str()) == 0)
                networkToUpdate = &this->config.networks[i];

            //! push_back creates a copy of the object, so we need to use emplace_back
            if (networkToUpdate != nullptr)
            {
                this->config.networks.emplace_back(
                    networkName,
                    ssid,
                    password,
                    *channel,
                    false);
            }
            log_d("Updating wifi config");
        }
    }
    else
    {
        //! push_back creates a copy of the object, so we need to use emplace_back
        this->config.networks.emplace_back(
            networkName,
            ssid,
            password,
            *channel,
            false);
        networkToUpdate = &this->config.networks[0];
    }

    if (shouldNotify)
        this->notify(ObserverEvent::networksConfigUpdated);
}

void ProjectConfig::setAPWifiConfig(const std::string &ssid, const std::string &password, uint8_t *channel, bool adhoc, bool shouldNotify)
{
    this->config.ap_network.ssid.assign(ssid);
    this->config.ap_network.password.assign(password);
    this->config.ap_network.channel = *channel;
    this->config.ap_network.adhoc = adhoc;

    log_d("Updating access point config");
    if (shouldNotify)
        this->notify(ObserverEvent::networksConfigUpdated);
}

std::string ProjectConfig::DeviceConfig_t::toRepresentation()
{
    std::string json = Helpers::format_string(
        "device_config: {\"OTAPassword\": \"%s\", \"OTAPort\": %u}",
        this->OTAPassword.c_str(),
        this->OTAPort);
    return json;
}

std::string ProjectConfig::CameraConfig_t::toRepresentation()
{
    std::string json = Helpers::format_string(
        "camera_config: {\"vflip\": %d,\"framesize\": %d,\"href\": %d,\"quality\": %d,\"brightness\": %d}",
        this->vflip,
        this->framesize,
        this->href,
        this->quality,
        this->brightness);
    return json;
}

std::string ProjectConfig::WiFiConfig_t::toRepresentation()
{
    std::string json = Helpers::format_string(
        "wifi_config: {\"name\": \"%s\", \"ssid\": \"%s\", \"password\": \"%s\", \"channel\": %u, \"adhoc\": %s}",
        this->name.c_str(),
        this->ssid.c_str(),
        this->password.c_str(),
        this->channel,
        this->adhoc ? "true" : "false");
    return json;
}

std::string ProjectConfig::AP_WiFiConfig_t::toRepresentation()
{
    std::string json = Helpers::format_string(
        "ap_wifi_config: {\"ssid\": \"%s\", \"password\": \"%s\", \"channel\": %u, \"adhoc\": %s}",
        this->ssid.c_str(),
        this->password.c_str(),
        this->channel,
        this->adhoc ? "true" : "false");
    return json;
}

//**********************************************************************************************************************
//*
//!                                                Get Methods
//*
//**********************************************************************************************************************

ProjectConfig::DeviceConfig_t *ProjectConfig::getDeviceConfig() { return &this->config.device; }
ProjectConfig::CameraConfig_t *ProjectConfig::getCameraConfig() { return &this->config.camera; }
std::vector<ProjectConfig::WiFiConfig_t> *ProjectConfig::getWifiConfigs() { return &this->config.networks; }
ProjectConfig::AP_WiFiConfig_t *ProjectConfig::getAPWifiConfig() { return &this->config.ap_network; }
ProjectConfig::MDNSConfig_t *ProjectConfig::getMDNSConfig() { return &this->config.mdns; }