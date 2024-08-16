#pragma once
#ifndef TASK_MANAGER_HPP
#define TASK_MANAGER_HPP
#include <ArduinoJson.h>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>

#include <iostream>
#include <iterator>
#include <memory>
#include <sstream>
#include <vector>

#include "data/CommandManager/Command.hpp"
#include "data/config/project_config.hpp"

struct CommandsPayload {
  JsonVariant data;
};

enum CommandType {
  None,
  PING,
  SET_WIFI,
  SET_MDNS,
  SAVE_CONFIG,
};

const std::unordered_map<std::string, CommandType> commandMap = {
    {"ping", CommandType::PING},
    {"set_wifi", CommandType::SET_WIFI},
    {"set_mdns", CommandType::SET_MDNS},
    {"save_config", CommandType::SAVE_CONFIG}};

class CommandManager {
 private:
  ProjectConfig& projectConfig;

  std::string join_strings(std::vector<std::string> const& strings,
                           std::string delim) {
    std::stringstream ss;
    std::copy(strings.begin(), strings.end(),
              std::ostream_iterator<std::string>(ss, delim.c_str()));
    return ss.str();
  }

  bool hasDataField(JsonVariant& command);
  std::unique_ptr<ICommand> createCommand(CommandType commandType,
                                          JsonVariant& data);

  std::variant<std::unique_ptr<ICommand>, CommandResult>
  createCommandFromJsonVariant(JsonVariant& command);

  CommandType getCommandType(JsonVariant& command);

  //   // TODO rewrite the API
  //   // TODO add FPS/ Freq / cropping to the API
  //   // TODO rewrite camera handler to be simpler and easier to change

 public:
  CommandManager(ProjectConfig& projectConfig)
      : projectConfig(projectConfig) {};

  CommandResult handleSingleCommand(CommandsPayload commandsPayload);
  std::variant<std::vector<CommandResult>, CommandResult> handleBatchCommands(
      CommandsPayload commandsPayload);
};
#endif