#include "data/CommandManager/BaseCommand.hpp"

// todo: make use of the update
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

class SetDeviceConfigCommand : public ICommand {
  ProjectConfig& projectConfig;
  JsonVariant data;

 public:
  SetDeviceConfigCommand(ProjectConfig& projectConfig, JsonVariant data)
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

class SetFPSCommand : public ICommand {
  ProjectConfig& projectConfig;
  JsonVariant data;

 public:
  SetFPSCommand(ProjectConfig& projectConfig, JsonVariant data)
      : projectConfig(projectConfig), data(data) {}

  CommandResult validate() override;
  CommandResult execute() override;
};
