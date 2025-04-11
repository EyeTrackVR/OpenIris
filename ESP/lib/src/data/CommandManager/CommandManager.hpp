#pragma once
#ifndef TASK_MANAGER_HPP
#define TASK_MANAGER_HPP
#include <ArduinoJson.h>
#include <unordered_map>
#include "data/config/project_config.hpp"

enum CommandType {
  None,
  PING,
  SET_WIFI,
  SET_MDNS,
  SWITCH_MODE,
  WIPE_WIFI_CREDS,
  RESTART_DEVICE,
};

struct CommandsPayload {
  JsonDocument data;
};

class CommandManager {
 private:
  const std::unordered_map<std::string, CommandType> commandMap = {
      {"ping", CommandType::PING},
      {"set_wifi", CommandType::SET_WIFI},
      {"set_mdns", CommandType::SET_MDNS},
      {"switch_mode", CommandType::SWITCH_MODE},
      {"wipe_wifi_creds", CommandType::WIPE_WIFI_CREDS},
      {"restart_device", CommandType::RESTART_DEVICE},
  };

  ProjectConfig* deviceConfig;

  bool hasDataField(JsonVariant& command);
  void handleCommand(JsonVariant command);
  const CommandType getCommandType(JsonVariant& command);

 public:
  CommandManager(ProjectConfig* deviceConfig);
  void handleCommands(CommandsPayload commandsPayload);
  ProjectConfig* getDeviceConfig() { return deviceConfig; }
};

#endif