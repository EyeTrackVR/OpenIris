#include "CommandManager.hpp"

std::unique_ptr<ICommand> CommandManager::createCommand(CommandType commandType,
                                                        JsonVariant& data) {
  switch (commandType) {
    case CommandType::PING:
      return std::make_unique<PingCommand>();
    case CommandType::SET_WIFI:
      return std::make_unique<SetWiFiCommand>(this->projectConfig, data);
    case CommandType::SET_MDNS:
      return std::make_unique<SetMDNSCommand>(this->projectConfig, data);
    case CommandType::SAVE_CONFIG:
      return std::make_unique<SaveConfigCommand>(this->projectConfig);
  }
}

CommandType CommandManager::getCommandType(JsonVariant& command) {
  if (!command.containsKey("command"))
    return CommandType::None;

  if (auto search = commandMap.find(command["command"]);
      search != commandMap.end())
    return search->second;

  return CommandType::None;
}

bool CommandManager::hasDataField(JsonVariant& command) {
  return command.containsKey("data");
}

std::variant<std::vector<CommandResult>, CommandResult>
CommandManager::handleBatchCommands(CommandsPayload commandsPayload) {
  std::vector<CommandResult> results = {};
  std::vector<std::string> errors = {};
  std::vector<std::unique_ptr<ICommand>> commands;

  if (!commandsPayload.data.containsKey("commands")) {
    std::string error = "Json data sent not supported, lacks commands field";
    log_e("%s", error.c_str());
    return CommandResult::getErrorResult(error);
  }

  for (JsonVariant commandData :
       commandsPayload.data["commands"].as<JsonArray>()) {
    auto command_or_result = this->createCommandFromJsonVariant(commandData);

    if (auto command_ptr =
            std::get_if<std::unique_ptr<ICommand>>(&command_or_result)) {
      auto validation_result = (*command_ptr)->validate();
      if (validation_result.isSuccess())
        commands.emplace_back(std::move((*command_ptr)));
      else
        errors.push_back(validation_result.getErrorMessage());
    } else {
      errors.push_back(
          std::get<CommandResult>(command_or_result).getErrorMessage());
      continue;
    }
  }

  // if we have any errors, consolidate them into a single message and return
  if (errors.size() > 0) {
    return CommandResult::getErrorResult(
        Helpers::format_string("\"[%s]\"", this->join_strings(errors, ", ")));
  }

  for (auto& valid_command : commands) {
    results.push_back(valid_command->execute());
  }

  return results;
}

CommandResult CommandManager::handleSingleCommand(
    CommandsPayload commandsPayload) {
  if (!commandsPayload.data.containsKey("command")) {
    std::string error = "Json data sent not supported, lacks commands field";
    log_e("%s", error.c_str());

    CommandResult::getErrorResult(error);
  }

  JsonVariant commandData = commandsPayload.data;
  auto command_or_result = this->createCommandFromJsonVariant(commandData);

  if (std::holds_alternative<CommandResult>(command_or_result)) {
    return std::get<CommandResult>(command_or_result);
  }

  auto command =
      std::move(std::get<std::unique_ptr<ICommand>>(command_or_result));

  auto validation_result = command->validate();
  if (!validation_result.isSuccess()) {
    return validation_result;
  };

  return command->execute();
}

std::variant<std::unique_ptr<ICommand>, CommandResult>
CommandManager::createCommandFromJsonVariant(JsonVariant& command) {
  auto command_type = this->getCommandType(command);
  if (command_type == CommandType::None) {
    std::string error =
        Helpers::format_string("Command not supported: %s", command["command"]);
    log_e("%s", error.c_str());
    return CommandResult::getErrorResult(error);
  }

  if (!this->hasDataField(command)) {
    std::string error = Helpers::format_string(
        "Command is missing data field: %s", command["command"]);
    log_e("%s", error.c_str());
    return CommandResult::getErrorResult(error);
  }

  auto command_data = command["data"].as<JsonVariant>();
  return this->createCommand(command_type, command_data);
}