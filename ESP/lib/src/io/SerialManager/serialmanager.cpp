#include "serialmanager.hpp"

SerialManager::SerialManager(ProjectConfig *projectConfig) : projectConfig(projectConfig),
                                                              serialManagerActive(false),
                                                              newData(false),
                                                              tempBuffer{0},
                                                              serialBuffer{0},
                                                              device_config_name{0},
                                                              device_config_OTAPassword{0},
                                                              device_config_OTAPort(0),
                                                              camera_config_vflip{0},
                                                              camera_config_href{0},
                                                              camera_config_framesize{0},
                                                              camera_config_quality{0},
                                                              wifi_config_name{0},
                                                              wifi_config_ssid{0},
                                                              wifi_config_password{0} {}

SerialManager::~SerialManager() {}

void SerialManager::listenToSerial(unsigned long timeout)
{
    log_d("Listening to serial");
    serialManagerActive = true;
    Serial.setTimeout(timeout);

    static bool recvInProgress = false;
    static uint8_t index = 0;  // index
    char startDelimiter = '<'; //! we need to decide on a delimiter for the start of a message
    char endDelimiter = '>';   //! we need to decide on a delimiter for the end of a message
    char receivedChar;         // to test for received data on the line

    while ((Serial.available() > 0) && !newData)
    {
        serialManagerActive = true;
        receivedChar = Serial.read();
        if (recvInProgress)
        {
            if (receivedChar != endDelimiter)
            {
                serialBuffer[index] = receivedChar;
                index++;
                if (index >= sizeof(serialBuffer))
                {
                    log_e("Serial buffer overflow");
                    index = 0;
                    recvInProgress = false;
                }
            }
            else
            {
                recvInProgress = false;
                serialBuffer[index] = '\0';
                index = 0;
                newData = true;
            }
        }
        else
        {
            if (receivedChar == startDelimiter)
            {
                recvInProgress = true;
            }
        }
    }
    serialManagerActive = false;
}

void SerialManager::parseData()
{
    log_d("Parsing data");
    char *strtokIndx; // this is used by strtok() as an index

    //! Parse the data
    //* Device Config *//
    strtokIndx = strtok(tempBuffer, ",");   // get the first part
    strcpy(device_config_name, strtokIndx); // copy it to buffer

    strtokIndx = strtok(NULL, ","); // get the second part
    strcpy(device_config_OTAPassword, strtokIndx);

    strtokIndx = strtok(NULL, ",");
    device_config_OTAPort = atoi(strtokIndx);

    //* Camera Config *//
    strtokIndx = strtok(NULL, ",");
    camera_config_vflip = atoi(strtokIndx);

    strtokIndx = strtok(NULL, ",");
    camera_config_framesize = atoi(strtokIndx);

    strtokIndx = strtok(NULL, ",");
    camera_config_href = atoi(strtokIndx);

    strtokIndx = strtok(NULL, ",");
    camera_config_quality = atoi(strtokIndx);

    //* Wifi Config *//
    strtokIndx = strtok(tempBuffer, ",");
    strcpy(wifi_config_name, strtokIndx);

    strtokIndx = strtok(NULL, ",");
    strcpy(wifi_config_ssid, strtokIndx);

    strtokIndx = strtok(NULL, ",");
    strcpy(wifi_config_password, strtokIndx);
}

void SerialManager::handleSerial()
{
    listenToSerial(30000L); // test for serial input every 30 seconds
    if (newData)            // input received
    {
        strcpy(tempBuffer, serialBuffer);                                                                                                 // this temporary copy is necessary to protect the original data because strtok() used in parseData() replaces the commas with \0
        parseData();                                                                                                                      // split the data into tokens and store them in the data structure
        projectConfig->setDeviceConfig(device_config_name, device_config_OTAPassword, &device_config_OTAPort, true);                       // set the values in the project config
        projectConfig->setCameraConfig(&camera_config_vflip, &camera_config_framesize, &camera_config_href, &camera_config_quality, true); // set the values in the project config
        projectConfig->setWifiConfig(wifi_config_name, wifi_config_ssid, wifi_config_password, true);                                      // set the values in the project config
        projectConfig->save();                                                                                                             // save the config to the EEPROM
        newData = false;                                                                                                                  // reset new data
    }
}