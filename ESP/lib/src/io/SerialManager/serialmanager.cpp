#include "serialmanager.hpp"

std::unordered_map<std::string, SerialManager::Serial_Commands> SerialManager::command_map = {
    {"", NO_INPUT},
    {"device_config", DEVICE_CONFIG},
    {"camera_config", CAMERA_CONFIG},
    {"wifi_config", WIFI_CONFIG}};

void readStr(const char *inStr);

SerialManager::SerialManager(ProjectConfig *projectConfig) : projectConfig(projectConfig),
                                                             serReader(std::make_unique<serialStr>())
{
}

SerialManager::~SerialManager() {}

void SerialManager::begin()
{
    serReader->setCallback(readStr);
}

void readStr(const char *inStr)
{
    Serial.print("command : ");
    Serial.println(inStr);
    std::string raw = inStr;
    std::vector<std::string> command;
    Helpers::split(raw, ":", command); //! gives us the command and the value - "command:value"
    std::vector<std::string> command_value;
    Helpers::split(command[1], ",", command_value); //! gives us the command and the value - "command:value"

    //! The following line uses strdup to return a char* to lwrCase
    char *lwr_case = strdup(command[0].c_str());
    lwrCase(lwr_case); //! converts the command to lowercase

    switch (SerialManager::command_map[lwr_case])
    {
    case SerialManager::NO_INPUT:
        break;
    case SerialManager::DEVICE_CONFIG:
        break;
    case SerialManager::CAMERA_CONFIG:
        break;
    case SerialManager::WIFI_CONFIG:
        break;
    }
}

void SerialManager::handleSerial()
{
    if (Serial.available() > 0)
    {
        delay(10);
        std::string raw = Serial.readStringUntil('#').c_str();
        // String s = "{\"a\":\"b\"}";

        while (Serial.available() > 0)
        {
            Serial.read();
        }
        log_d("Received Serial Data: %s", raw.c_str());

        DeserializationError error = deserializeJson(jsonDoc, raw);
        if (error)
        {
            log_e("deserializeJson() failed: %s", error.c_str());
            return;
        }

        const char *device_config_name = jsonDoc["device_config_name"];
        const char *device_config_OTAPassword = jsonDoc["device_config_OTAPassword"];
        const char *device_config_OTAPort = jsonDoc["device_config_OTAPort"];
        const char *camera_config_vflip = jsonDoc["camera_config_vflip"];
        const char *camera_config_href = jsonDoc["camera_config_href"];
        const char *camera_config_framesize = jsonDoc["camera_config_framesize"];
        const char *camera_config_quality = jsonDoc["camera_config_quality"];
        const char *wifi_config_name = jsonDoc["wifi_config_name"];
        const char *wifi_config_ssid = jsonDoc["wifi_config_ssid"];
        const char *wifi_config_password = jsonDoc["wifi_config_password"];
        const char *wifi_config_channel = jsonDoc["wifi_config_channel"];
    }
}