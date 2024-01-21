#include "CommandManager.hpp"

CommandManager::CommandManager(ProjectConfig *deviceConfig) : deviceConfig(deviceConfig) {}


const CommandType CommandManager::getCommandType(Command &command){ 
    if (!command.data.containsKey("command"))
        return CommandType::None;

    if (auto search = commandMap.find(command.data["command"]); search != commandMap.end())
        return search->second;
    
    return CommandType::None;
}


void CommandManager::handleCommand(Command command) {
    auto command_type = this->getCommandType(command); 

    if (!command.data.containsKey("data"))
        // malformed command, lacked data field
        return; 

    switch(command_type)
    {
    case CommandType::SET_WIFI: {
        if(!command.data["data"].containsKey("ssid") || !command.data["data"].containsKey("password"))
            break;

        std::string customNetworkName = "main";
        if (command.data["data"].containsKey("network_name"))
            customNetworkName = command.data["data"]["network_name"].as<std::string>();

        this->deviceConfig->setWifiConfig(
            customNetworkName,
            command.data["data"]["ssid"],
            command.data["data"]["password"],
            0, // channel, should this be zero?
            0, // power, should this be zero? 
            false, 
            false
        );
        
        // we purposefully save here
        this->deviceConfig->save();
        break;
    }
    case CommandType::PING: {
        Serial.println("PONG \n\r");
        break;
    }
    default:
        break;
    }
}