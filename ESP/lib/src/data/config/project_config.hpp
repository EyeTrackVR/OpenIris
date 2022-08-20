#pragma once
#ifndef PROJECT_CONFIG_HPP
#define PROJECT_CONFIG_HPP
#include <Arduino.h>
#include <preferencesAPI.hpp>
#include <vector>
#include <string>

#include "data/utilities/Observer.hpp"

class ProjectConfig : public Config, public ISubject
{
public:
    ProjectConfig();
    virtual ~ProjectConfig();
    void load();
    void save();
    void reset();
    void initConfig();

    struct DeviceConfig_t
    {
        std::string name;
        std::string OTAPassword;
        int OTAPort;
        bool data_json;
        bool config_json;
        bool settings_json;
        String data_json_string;
        String config_json_string;
        String settings_json_string;
    };

    struct CameraConfig_t
    {
        uint8_t vflip;
        uint8_t framesize;
        uint8_t href;
        uint8_t quality;
    };

    struct WiFiConfig_t
    {
        std::string name;
        std::string ssid;
        std::string password;
        uint8_t channel;
        bool adhoc;
    };

    struct AP_WiFiConfig_t
    {
        std::string ssid;
        std::string password;
        uint8_t channel;
        bool adhoc;
    };

    struct TrackerConfig_t
    {
        DeviceConfig_t device;
        CameraConfig_t camera;
        std::vector<WiFiConfig_t> networks;
        AP_WiFiConfig_t ap_network;
    };

    DeviceConfig_t *getDeviceConfig() { return &this->config.device; }
    CameraConfig_t *getCameraConfig() { return &this->config.camera; }
    std::vector<WiFiConfig_t> *getWifiConfigs() { return &this->config.networks; }
    AP_WiFiConfig_t *getAPWifiConfig() { return &this->config.ap_network; }

    void setDeviceConfig(const char *name, const char *OTAPassword, int *OTAPort, bool shouldNotify);
    void setCameraConfig(uint8_t *vflip, uint8_t *framesize, uint8_t *href, uint8_t *quality, bool shouldNotify);
    void setWifiConfig(const char *networkName, const char *ssid, const char *password, uint8_t *channel, bool adhoc, bool shouldNotify);
    void setAPWifiConfig(const char *ssid, const char *password, uint8_t *channel, bool adhoc, bool shouldNotify);

private:
    const char *configFileName;
    TrackerConfig_t config;
    bool _already_loaded;
};

#endif // PROJECT_CONFIG_HPP