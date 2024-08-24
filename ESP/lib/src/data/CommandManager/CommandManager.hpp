#pragma once
#ifndef TASK_MANAGER_HPP
#define TASK_MANAGER_HPP
#include <ArduinoJson.h>

#include <iostream>
#include <iterator>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

#include "data/CommandManager/BaseCommand.hpp"
#include "data/config/project_config.hpp"
#include "network/stream/streamServer.hpp"

struct CommandsPayload {
  JsonVariant data;
};

enum CommandType {
  None,
  PING,
  SET_WIFI,
  SET_MDNS,
  SET_FPS,
  TOGGLE_STREAM,
  SAVE_CONFIG,
};

const std::unordered_map<std::string, CommandType> commandMap = {
    {"ping", CommandType::PING},
    {"set_wifi", CommandType::SET_WIFI},
    {"set_mdns", CommandType::SET_MDNS},
    {"set_fps", CommandType::SET_FPS},
    {"toggle_stream", CommandType::TOGGLE_STREAM},
    {"save_config", CommandType::SAVE_CONFIG}};

class CommandManager {
 private:
  ProjectConfig &projectConfig;
  StreamServer &streamServer;

  std::string join_strings(std::vector<std::string> const &strings,
                           std::string delim) {
    std::stringstream ss;
    std::copy(strings.begin(), strings.end(),
              std::ostream_iterator<std::string>(ss, delim.c_str()));
    return ss.str();
  }

  bool hasDataField(JsonVariant &command);
  std::unique_ptr<ICommand> createCommand(CommandType commandType,
                                          JsonVariant &data);

  std::variant<std::unique_ptr<ICommand>, CommandResult>
  createCommandFromJsonVariant(JsonVariant &command);

  CommandType getCommandType(JsonVariant &command);

  //   // TODO rewrite the API
  //   // TODO add FPS/ Freq / cropping to the API
  //   // TODO rewrite camera handler to be simpler and easier to change

 public:
  CommandManager(ProjectConfig &projectConfig, StreamServer &streamServer)
      : projectConfig(projectConfig), streamServer(streamServer) {};

  CommandResult handleSingleCommand(CommandsPayload commandsPayload);
  CommandResult handleBatchCommands(CommandsPayload commandsPayload);
};
#endif