#include "serialmanager.hpp"

SerialManager::SerialManager() : serialManagerActive(false),
                                 newData(false),
                                 tempBuffer{0},
                                 serialBuffer{0},
                                 device_config_name{0},
                                 device_config_OTAPassword{0},
                                 device_config_OTAPort(0) {}

SerialManager::~SerialManager() {}

void SerialManager::listenToSerial(int timeout)
{
    log_d("Listening to serial");
    serialManagerActive = true;
    Serial.setTimeout(timeout);

    static boolean recvInProgress = false;
    static byte index = 0; // index
    char startDelimiter = '<';
    char endDelimiter = '>';
    char receivedChar; // to test for received data on the line

    while (serialManagerActive)
    {
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

        if (Serial.available() > 0)
        {
            Serial.readBytesUntil('\n', this->serialBuffer, sizeof(this->serialBuffer));
        }
        delay(timeout);
        serialManagerActive = false;
    }
}

void SerialManager::parseData()
{
    log_d("Parsing data");
    char *strtokIndx; // this is used by strtok() as an index

    strtokIndx = strtok(tempBuffer, ",");   // get the first part - the string
    strcpy(device_config_name, strtokIndx); // copy it to buffer

    strtokIndx = strtok(NULL, ",");         // get the second part - the string
    strcpy(device_config_OTAPassword, strtokIndx); // copy it to buffer

    strtokIndx = strtok(NULL, ",");   // get the first part - the string
    device_config_OTAPort = atoi(strtokIndx); // convert this part to an integer

    projectConfig.setDeviceConfig( ); // get the second part - the value
    if (newData)
    {
        log_d("New data");
        newData = false;
        char *token = strtok(serialBuffer, ",");
        while (token != NULL)
        {
            log_d("Token: %s", token);
            token = strtok(NULL, ",");
        }
    }
}

void SerialManager::moveData()
{
    listenToSerial(30000); // test for serial input for 30 seconds
    if (newData)           // input received
    {
        strcpy(tempBuffer, serialBuffer); // this temporary copy is necessary to protect the original data because strtok() used in parseData() replaces the commas with \0
        parseData();                      // split the data into tokens and store them in the data structure
        newData = false;                  // reset new data
    }
}
