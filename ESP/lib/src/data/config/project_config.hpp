#pragma once
#ifndef PROJECT_CONFIG_HPP
#define PROJECT_CONFIG_HPP
#include <Arduino.h>
#include <Preferences.h>
#include <algorithm>
#include <string>
#include <vector>

#include "data/StateManager/StateManager.hpp"
#include "data/utilities/Observer.hpp"
#include "data/utilities/helpers.hpp"
#include "data/utilities/network_utilities.hpp"
#include "tasks/tasks.hpp"

class ProjectConfig : public Preferences, public ISubject<ConfigState_e> {
 public:
  ProjectConfig(const std::string& name = std::string(),
                const std::string& mdnsName = std::string());
  virtual ~ProjectConfig();
  void load();
  void save();
  void wifiConfigSave();
  void cameraConfigSave();
  void deviceConfigSave();
  void mdnsConfigSave();
  void wifiTxPowerConfigSave();
  bool reset();
  void initConfig();

  struct DeviceConfig_t {
    std::string OTALogin;
    std::string OTAPassword;
    int OTAPort;
    std::string toRepresentation();
  };

  struct MDNSConfig_t {
    std::string hostname;
    std::string service;
    std::string toRepresentation();
  };

  struct CameraConfig_t {
    uint8_t vflip;
    uint8_t href;
    uint8_t framesize;
    uint8_t quality;
    uint8_t brightness;

    std::string toRepresentation();
  };

  struct WiFiConfig_t {
    //! Constructor for WiFiConfig_t - allows us to use emplace_back
    WiFiConfig_t(const std::string& name,
                 const std::string& ssid,
                 const std::string& password,
                 uint8_t channel,
                 uint8_t power,
                 bool adhoc)
        : name(std::move(name)),
          ssid(std::move(ssid)),
          password(std::move(password)),
          channel(channel),
          adhoc(adhoc),
          power(power) {}
    std::string name;
    std::string ssid;
    std::string password;
    uint8_t channel;
    uint8_t power;
    bool adhoc;

    std::string toRepresentation();
  };

  struct AP_WiFiConfig_t {
    std::string ssid;
    std::string password;
    uint8_t channel;
    bool adhoc;
    std::string toRepresentation();
  };

  struct WiFiTxPower_t {
    uint8_t power;
    std::string toRepresentation();
  };

  struct TrackerConfig_t {
    DeviceConfig_t device;
    CameraConfig_t camera;
    std::vector<WiFiConfig_t> networks;
    AP_WiFiConfig_t ap_network;
    MDNSConfig_t mdns;
    WiFiTxPower_t txpower;
  };

  DeviceConfig_t& getDeviceConfig();
  CameraConfig_t& getCameraConfig();
  std::vector<WiFiConfig_t>& getWifiConfigs();
  AP_WiFiConfig_t& getAPWifiConfig();
  MDNSConfig_t& getMDNSConfig();
  WiFiTxPower_t& getWiFiTxPowerConfig();

  void setDeviceConfig(const std::string& OTALogin,
                       const std::string& OTAPassword,
                       int OTAPort,
                       bool shouldNotify);
  void setMDNSConfig(const std::string& hostname,
                     const std::string& service,
                     bool shouldNotify);
  void setCameraConfig(uint8_t vflip,
                       uint8_t framesize,
                       uint8_t href,
                       uint8_t quality,
                       uint8_t brightness,
                       bool shouldNotify);
  void setWifiConfig(const std::string& networkName,
                     const std::string& ssid,
                     const std::string& password,
                     uint8_t channel,
                     uint8_t power,
                     bool adhoc,
                     bool shouldNotify);
  void setAPWifiConfig(const std::string& ssid,
                       const std::string& password,
                       uint8_t channel,
                       bool adhoc,
                       bool shouldNotify);
  void setWiFiTxPower(uint8_t power, bool shouldNotify);

  void deleteWifiConfig(const std::string& networkName, bool shouldNotify);

 private:
  TrackerConfig_t config;
  std::string _name;
  std::string _mdnsName;
  bool _already_loaded;
};

#endif  // PROJECT_CONFIG_HPP
