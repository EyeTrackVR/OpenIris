#pragma once
#ifndef TASK_MANAGER_HPP
#define TASK_MANAGER_HPP
#include <ArduinoJson.h>
#include <unordered_map>
#include "data/config/project_config.hpp"

enum CommandType {
    None,
    PING,
    SET_WIFI,
    SET_MDNS,
};


struct Command { 
    JsonDocument data;
};

class CommandManager{
    
private:
const std::unordered_map<std::string, CommandType> commandMap = {
    {"ping", CommandType::PING},
    {"set_wifi", CommandType::SET_WIFI},
    {"set_mdns", CommandType::SET_MDNS},
};

ProjectConfig* deviceConfig;

bool hasHasDataField(Command &command);

public:
    CommandManager(ProjectConfig *deviceConfig); 
    void handleCommand(Command command);
    const CommandType getCommandType(Command &command);
};

#endif