#pragma once
#include <Arduino.h>

#include "data/config/project_config.hpp"

class SerialManager
{
public:
    SerialManager();
    virtual ~SerialManager();

    void listenToSerial(int timeout);
    void parseData();
    void moveData();

    bool serialManagerActive;

    char device_config_name[32];
    char device_config_OTAPassword[100];
    int device_config_OTAPort;

private:
    enum DataTypes_e
    {
        DataType_Unknown,
        DataType_Device,
        DataType_Camera,
        DataType_Wifi,
        DataType_Error,
        DataType_Debug
    };

    char tempBuffer[sizeof(serialBuffer) / sizeof(serialBuffer[0])];
    char serialBuffer[100000]; //! Need to find the appropriate size for this - count the maximum possible size of a message
    bool newData;
    
};

extern SerialManager serialManager;