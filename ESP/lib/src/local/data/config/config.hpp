#pragma once
#ifndef OPENIRIS_CONFIG_HPP
#define OPENIRIS_CONFIG_HPP
#include <string>

#include <utilities/helpers.hpp>
#include <utilities/network_utilities.hpp>
#include "local/tasks/tasks.hpp"

#include <data/config/project_config.hpp>
#include <local/data/statemanager/state_manager.hpp>
#include <unordered_map>

class OpenIrisConfig : public CustomConfigInterface,
                       public ISubject<ConfigState_e> {
  ProjectConfig& projectConfig;

 public:
  OpenIrisConfig(ProjectConfig& projectConfig);
  virtual ~OpenIrisConfig();

  //* CustomConfigInterface
  void load() override;
  void save() override;

  //* load
  void cameraConfigLoad();
  void initConfig();

  //* Save
  void cameraConfigSave();

  struct CameraConfig_t {
    uint8_t vflip;
    uint8_t href;
    uint8_t framesize;
    uint8_t quality;
    uint8_t brightness;

    std::string toRepresentation();
  };

  struct TrackerConfig_t {
    CameraConfig_t camera;
  };

  CameraConfig_t& getCameraConfig();
  void setCameraConfig(uint8_t vflip,
                       uint8_t framesize,
                       uint8_t href,
                       uint8_t quality,
                       uint8_t brightness,
                       bool shouldNotify);

 private:
  TrackerConfig_t config;
};

#endif  // PROJECT_CONFIG_HPP
