#pragma once
#ifndef PROJECT_CONFIG_HPP
#define PROJECT_CONFIG_HPP
#include <Arduino.h>
#include <preferencesAPI.hpp>
#include <vector>

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
        const char *name;
        const char *OTAPassword;
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
        const char *name;
        const char *ssid;
        const char *password;
    };

    struct TrackerConfig_t
    {
        DeviceConfig_t device;
        CameraConfig_t camera;
        std::vector<WiFiConfig_t> networks;
    };

    DeviceConfig_t *getDeviceConfig() { return &this->config.device; }
    CameraConfig_t *getCameraConfig() { return &this->config.camera; }
    std::vector<WiFiConfig_t> *getWifiConfigs() { return &this->config.networks; }

    void setDeviceConfig(const char *name, const char *OTAPassword, int *OTAPort, bool shouldNotify);
    void setCameraConfig(uint8_t *vflip, uint8_t *framesize, uint8_t *href, uint8_t *quality, bool shouldNotify);
    void setWifiConfig(const char *networkName, const char *ssid, const char *password, bool shouldNotify);
    
private:
    const char *configFileName;
    TrackerConfig_t config;
    bool _already_loaded;
};

#endif // PROJECT_CONFIG_HPP