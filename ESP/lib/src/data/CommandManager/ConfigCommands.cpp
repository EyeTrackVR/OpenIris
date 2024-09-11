#include "ConfigCommands.hpp"

CommandResult SetWiFiCommand::validate() {
  if (!data.containsKey("ssid"))
    return CommandResult::getErrorResult("Missing ssid");
  if (!data.containsKey("password"))
    return CommandResult::getErrorResult("Missing password");
  return CommandResult::getSuccessResult("");
}

CommandResult SetWiFiCommand::execute() {
  std::string network_name = "main";
  if (data.containsKey("network_name"))
    network_name = data["network_name"].as<std::string>();

  projectConfig.setWifiConfig(network_name, data["ssid"], data["password"], 0,
                              0, false, false);
  return CommandResult::getSuccessResult("WIFI Set to: " +
                                         data["ssid"].as<std::string>());
}

CommandResult SetMDNSCommand::validate() {
  if (!data.containsKey("hostname") || !strlen(data["hostname"]))
    return CommandResult::getErrorResult("Missing hostname");

  return CommandResult::getSuccessResult("");
}

CommandResult SetMDNSCommand::execute() {
  projectConfig.setMDNSConfig(data, false);
  return CommandResult::getSuccessResult("MDNS set to:" +
                                         data["hostname"].as<std::string>());
}

CommandResult SetDeviceConfigCommand::validate() {
  return CommandResult::getSuccessResult("");
}

CommandResult SetDeviceConfigCommand::execute() {
  projectConfig.setDeviceConfig(data, false);
  return CommandResult::getSuccessResult("Device config updated.");
}

CommandResult SaveConfigCommand::execute() {
  projectConfig.save();
  return CommandResult::getSuccessResult("CONFIG SAVED");
}

CommandResult SetFPSCommand::validate() {
  if (!data.containsKey("fps") || !data["hostname"])
    return CommandResult::getErrorResult("Missing fps or FPS were negative");

  return CommandResult::getSuccessResult("");
}

CommandResult SetFPSCommand::execute() {
  projectConfig.setCameraConfig("fps", data["fps"]);
  return CommandResult::getSuccessResult("FPS set to:" +
                                         data["fps"].as<std::string>());
}