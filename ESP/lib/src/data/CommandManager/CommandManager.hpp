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
  };

  ProjectConfig* deviceConfig;

  bool hasDataField(JsonVariant& command);
  void handleCommand(JsonVariant command);
  const CommandType getCommandType(JsonVariant& command);

 public:
  CommandManager(ProjectConfig* deviceConfig);
  void handleCommands(CommandsPayload commandsPayload);
};

#endif