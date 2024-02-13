#pragma once
#ifndef WIFIHANDLER_HPP
#define WIFIHANDLER_HPP
#include <string>
#include "data/config/project_config.hpp"
#include "data/utilities/Observer.hpp"

class WiFiHandler : public IObserver<ConfigState_e> {
 public:
  WiFiHandler(ProjectConfig& configManager,
              const std::string& ssid,
              const std::string& password,
              uint8_t channel,
              bool enable_adhoc);
  virtual ~WiFiHandler();
  void begin();
  void update(ConfigState_e event) override;
  std::string getName() override;

 private:
  void setUpADHOC();
  void adhoc(const std::string& ssid,
             uint8_t channel,
             const std::string& password = std::string());
  bool iniSTA(const std::string& ssid,
              const std::string& password,
              uint8_t channel,
              wifi_power_t power);

  ProjectConfig& configManager;


  bool _enable_adhoc;
  std::string ssid;
  std::string password;
  uint8_t channel;
  uint8_t power;
};
#endif  // WIFIHANDLER_HPP
