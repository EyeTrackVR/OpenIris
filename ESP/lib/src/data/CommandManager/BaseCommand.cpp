#include "BaseCommand.hpp"

CommandResult PingCommand::execute() {
  return CommandResult::getSuccessResult("pong");
}

CommandResult ToggleStreamCommand::validate() {
  if (!data.containsKey("state"))
    return CommandResult::getErrorResult("Missing state field");

  return CommandResult::getSuccessResult("");
}

CommandResult ToggleStreamCommand::execute() {
  this->streamServer.toggleTCPStream(data["state"].as<bool>());

  return CommandResult::getSuccessResult("TCP Stream state set to:" +
                                         data["state"].as<std::string>());
}