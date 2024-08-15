#ifndef COMMAND_HPP
#define COMMAND_HPP
#include <ArduinoJson.h>
#include <optional>
#include <string>
#include <variant>
#include "data/config/project_config.hpp"

class CommandResult {
 private:
  // or maybe std::optional?
  std::optional<std::string> successMessage;
  std::optional<std::string> errorMessage;

 public:
  CommandResult(std::optional<std::string> successMessage,
                std::optional<std::string> errorMessage)
      : successMessage(successMessage), errorMessage(errorMessage) {}

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

class SetWiFiCommand : public ICommand {
  ProjectConfig& projectConfig;
  JsonVariant data;

 public:
  SetWiFiCommand(ProjectConfig& projectConfig, JsonVariant data)
      : projectConfig(projectConfig), data(data) {}
  CommandResult validate() override;
  CommandResult execute() override;
};

class SetMDNSCommand : public ICommand {
  ProjectConfig& projectConfig;
  JsonVariant data;

 public:
  SetMDNSCommand(ProjectConfig& projectConfig, JsonVariant data)
      : projectConfig(projectConfig), data(data) {}
  CommandResult validate() override;
  CommandResult execute() override;
};

class SaveConfigCommand : public ICommand {
  ProjectConfig& projectConfig;

 public:
  SaveConfigCommand(ProjectConfig& projectConfig)
      : projectConfig(projectConfig) {}

  CommandResult validate() override {
    return CommandResult::getSuccessResult("");
  };

  CommandResult execute() override;
};

#endif