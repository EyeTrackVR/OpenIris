#ifndef COMMAND_HPP
#define COMMAND_HPP
#include <ArduinoJson.h>

#include <optional>
#include <string>
#include <variant>

#include "data/config/project_config.hpp"
#include "network/stream/streamServer.hpp"

class CommandResult {
 private:
  std::optional<std::string> successMessage;
  std::optional<std::string> errorMessage;

 public:
  CommandResult(std::optional<std::string> success_message,
                std::optional<std::string> error_message) {
    if (success_message.has_value()) {
      this->successMessage =
          "{\"message\":\"" + success_message.value() + "\"}";
    } else
      this->successMessage = std::nullopt;

    if (error_message.has_value())
      this->errorMessage = "{\"error\":\"" + error_message.value() + "\"}";
    else
      this->errorMessage = std::nullopt;
  }

  bool isSuccess() const { return successMessage.has_value(); }

  static CommandResult getSuccessResult(std::string message) {
    return CommandResult(message, std::nullopt);
  }

  static CommandResult getErrorResult(std::string message) {
    return CommandResult(std::nullopt, message);
  }

  std::string getSuccessMessage() const { return successMessage.value(); };
  std::string getErrorMessage() const { return errorMessage.value(); }
};

class ICommand {
 public:
  virtual CommandResult validate() = 0;
  virtual CommandResult execute() = 0;
  virtual ~ICommand() = default;
};

class PingCommand : public ICommand {
 public:
  CommandResult validate() override {
    return CommandResult::getSuccessResult("");
  };
  CommandResult execute() override;
};

class ToggleStreamCommand : public ICommand {
  StreamServer& streamServer;
  JsonVariant data;

 public:
  ToggleStreamCommand(StreamServer& streamServer, JsonVariant data)
      : streamServer(streamServer), data(data) {}

  CommandResult validate() override;
  CommandResult execute() override;
};
#endif