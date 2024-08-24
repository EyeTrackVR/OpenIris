#pragma once
#ifndef PROJECT_CONFIG_HPP
#define PROJECT_CONFIG_HPP
#include <Arduino.h>
#include <ArduinoJson.h>
#include <Preferences.h>

#include <algorithm>
#include <string>
#include <variant>
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

    void update(std::string field, JsonVariant value) {
      // this technically could be done with a hashmap that takes
      // and std::function<void(JsonVariant)> and some lambda captures
      // but it seemed too unvieldy for the time being

      if (field == "OTALogin") {
        this->OTALogin = value.as<std::string>();
      } else if (field == "OTAPassword") {
        this->OTAPassword = value.as<std::string>();
      } else if (field == "OTAPort") {
        this->OTAPort = value.as<int>();
      }
    }
  };

  struct MDNSConfig_t {
    std::string hostname;
    std::string service;
    std::string toRepresentation();

    void update(std::string field, JsonVariant value) {
      if (field == "hostname") {
        this->hostname = std::move(value.as<std::string>());
      } else if (field == "service") {
        this->service = std::move(value.as<std::string>());
      }
    }
  };

  struct CameraConfig_t {
   public:
    uint8_t vflip;
    uint8_t href;
    uint8_t framesize;
    uint8_t quality;
    uint8_t brightness;
    uint8_t fps;

    std::string toRepresentation();

    void update(std::string field, JsonVariant value) {
      if (field == "vflip") {
        this->vflip = value.as<uint8_t>();
      } else if (field == "href") {
        this->href = value.as<uint8_t>();
      } else if (field == "framesize") {
        this->framesize = value.as<uint8_t>();
      } else if (field == "quality") {
        this->quality = value.as<uint8_t>();
      } else if (field == "brightness") {
        this->brightness = value.as<uint8_t>();
      } else if (field == "fps") {
        this->fps = value.as<uint8_t>();
      }
    }
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

    void update(std::string field, JsonVariant value) {
      if (field == "name") {
        this->name = std::move(value.as<std::string>());
      } else if (field == "ssid") {
        this->ssid = std::move(value.as<std::string>());
      } else if (field == "password") {
        this->password = std::move(value.as<std::string>());
      } else if (field == "channel") {
        this->channel = value.as<uint8_t>();
      } else if (field == "power") {
        this->power = value.as<uint8_t>();
      } else if (field == "adhoc") {
        this->adhoc = value.as<bool>();
      }
    }
  };

  struct AP_WiFiConfig_t {
    std::string ssid;
    std::string password;
    uint8_t channel;
    bool adhoc;
    std::string toRepresentation();
    void update(std::string field, JsonVariant value) {
      if (field == "ssid") {
        this->ssid = std::move(value.as<std::string>());
      } else if (field == "password") {
        this->password = std::move(value.as<std::string>());
      } else if (field == "channel") {
        this->channel = value.as<uint8_t>();
      } else if (field == "adhoc") {
        this->adhoc = value.as<bool>();
      }
    }
  };

  struct WiFiTxPower_t {
    uint8_t power;
    std::string toRepresentation();
    void update(uint8_t power) { this->power = power; }
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

  void setDeviceConfig(JsonVariant& data, bool shouldNotify);

  void setMDNSConfig(JsonVariant& data, bool shouldNotify);
  void setCameraConfig(JsonVariant& data, bool shouldNotify);
  void setWifiConfig(JsonVariant& data, bool shouldNotify);
  void setAPWifiConfig(JsonVariant& data, bool shouldNotify);

  void setWiFiTxPower(uint8_t power, bool shouldNotify);

  void deleteWifiConfig(const std::string& networkName, bool shouldNotify);

 private:
  TrackerConfig_t config;
  std::string _name;
  std::string _mdnsName;
  bool _already_loaded;
};

#endif  // PROJECT_CONFIG_HPP
