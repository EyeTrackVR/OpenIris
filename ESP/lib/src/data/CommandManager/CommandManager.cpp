#include "CommandManager.hpp"
#include "tasks/tasks.hpp"


CommandManager::CommandManager(ProjectConfig* deviceConfig)
    : deviceConfig(deviceConfig) {}

const CommandType CommandManager::getCommandType(JsonVariant& command) {
  if (!command["command"].is<const char*>())
    return CommandType::None;

  if (auto search = commandMap.find(command["command"]);
      search != commandMap.end())
    return search->second;

  return CommandType::None;
}

bool CommandManager::hasDataField(JsonVariant& command) {
  return command["data"].is<JsonObject>();
}

void CommandManager::handleCommands(CommandsPayload commandsPayload) {
  if (!commandsPayload.data["commands"].is<JsonArray>()) {
    log_e("Json data sent not supported, lacks commands field");
    return;
  }

  for (JsonVariant commandData :
       commandsPayload.data["commands"].as<JsonArray>()) {
    this->handleCommand(commandData);
  }

  this->deviceConfig->save();
}

void CommandManager::handleCommand(JsonVariant command) {
  auto command_type = this->getCommandType(command);

  switch (command_type) {
    case CommandType::SET_WIFI: {
      if (!this->hasDataField(command))
        // malformed command, lacked data field
        break;

      if (!command["data"]["ssid"].is<const char*>() ||
          !command["data"]["password"].is<const char*>())
        break;

      std::string customNetworkName = "main";
      if (command["data"]["network_name"].is<const char*>())
        customNetworkName = command["data"]["network_name"].as<std::string>();

      this->deviceConfig->setWifiConfig(customNetworkName,
                                        command["data"]["ssid"],
                                        command["data"]["password"],
                                        0,  // channel, should this be zero?
                                        0,  // power, should this be zero?
                                        false, false);
      
      this->deviceConfig->setHasWiFiCredentials(true, false);
      this->deviceConfig->setDeviceMode(DeviceMode::WIFI_MODE, true);
      log_i("[CommandManager] Switching to WiFi mode after receiving credentials");

      break;
    }
    case CommandType::SET_MDNS: {
      if (!this->hasDataField(command))
        break;

      if (!command["data"]["hostname"].is<const char*>() ||
          !strlen(command["data"]["hostname"]))
        break;

      this->deviceConfig->setMDNSConfig(command["data"]["hostname"],
                                        "openiristracker", false);

      break;
    }
    case CommandType::PING: {
      Serial.println("PONG \n\r");
      break;
    }
    case CommandType::SWITCH_MODE: {
      if (!this->hasDataField(command))
        break;
      
      if (!command["data"]["mode"].is<int>())
        break;
      
      int modeValue = command["data"]["mode"];
      DeviceMode newMode = static_cast<DeviceMode>(modeValue);
      DeviceMode currentMode = this->deviceConfig->getDeviceModeConfig().mode;
      
      // If switching to USB mode from WiFi or AP mode, disconnect WiFi immediately
      if (newMode == DeviceMode::USB_MODE && 
          (currentMode == DeviceMode::WIFI_MODE || currentMode == DeviceMode::AP_MODE)) {
        log_i("[CommandManager] Immediately switching to USB mode");
        WiFi.disconnect(true);
      }
      
      this->deviceConfig->setDeviceMode(newMode, true);
      log_i("[CommandManager] Switching to mode: %d", modeValue);
      
      // Removed automatic restart to allow explicit control via RESTART_DEVICE command
      // if (!(newMode == DeviceMode::USB_MODE && 
      //       (currentMode == DeviceMode::WIFI_MODE || currentMode == DeviceMode::AP_MODE) && 
      //       wifiStateManager.getCurrentState() == WiFiState_e::WiFiState_Connecting)) {
      //         OpenIrisTasks::ScheduleRestart(2000);
      // }
      
      break;
    }
    case CommandType::WIPE_WIFI_CREDS: {

      auto networks = this->deviceConfig->getWifiConfigs();
      for (auto& network : networks) {
        this->deviceConfig->deleteWifiConfig(network.name, false);
      }
      
      this->deviceConfig->setHasWiFiCredentials(false, false);
      this->deviceConfig->setDeviceMode(DeviceMode::USB_MODE, true);
      log_i("[CommandManager] Switching to USB mode after wiping credentials");
      // Removed automatic restart to allow processing of all commands in payload
      
      break;
    }
    case CommandType::RESTART_DEVICE: {
      log_i("[CommandManager] Explicit restart requested");
      OpenIrisTasks::ScheduleRestart(2000);
      break;
    }
    default:
      break;
  }
}