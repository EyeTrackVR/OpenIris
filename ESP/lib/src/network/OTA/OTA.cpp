#include "OTA.hpp"

OTA::OTA(ProjectConfig *_deviceConfig) : _deviceConfig(_deviceConfig) {}

OTA::~OTA() {}

void OTA::SetupOTA()
{
    log_e("Setting up OTA updates");
    auto localConfig = _deviceConfig->getDeviceConfig();

    if (strcmp(localConfig->OTAPassword.c_str(), "") == 0)
    {
        log_e("THE PASSWORD IS REQUIRED, [[ABORTING]]");
        return;
    }

    ArduinoOTA.setPort(localConfig->OTAPort);

    ArduinoOTA
        .onStart([]()
                 {
                String type;
                if (ArduinoOTA.getCommand() == U_FLASH)
                    type = "sketch";
                else // U_SPIFFS
                    type = "filesystem"; })
        .onEnd([]()
               { Serial.println("OTA updated finished successfully!"); })
        .onProgress([](unsigned int progress, unsigned int total)
                    { Serial.printf("Progress: %u%%\r", (progress / (total / 100))); })
        .onError([](ota_error_t error)
                 {
                log_e("Error[%u]: ", error);
                switch (error)
                {
                case OTA_AUTH_ERROR:
                    log_e("Auth Failed");
                    break;
                case OTA_BEGIN_ERROR:
                    log_e("Begin Failed");
                    break;
                case OTA_CONNECT_ERROR:
                    log_e("Connect Failed");
                    break;
                case OTA_RECEIVE_ERROR:
                    log_e("Receive Failed");
                    break;
                case OTA_END_ERROR:
                    log_e("End Failed");
                    break;
                } });

    log_i("Starting up basic OTA server");
    log_i("OTA will be live for 30s, after which it will be disabled until restart");
    ArduinoOTA.begin();
    _bootTimestamp = millis();
}

void OTA::HandleOTAUpdate()
{
    if (_isOtaEnabled)
    {
        if (_bootTimestamp + 30000 < millis())
        {
            // we're disabling ota after first 30sec so that nothing bad happens during runtime
            _isOtaEnabled = false;
            log_i("From now on, OTA is disabled");
            return;
        }
        ArduinoOTA.handle();
    }
}