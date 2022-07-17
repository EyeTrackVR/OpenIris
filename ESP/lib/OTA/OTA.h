#pragma once
#include <ArduinoOTA.h>

class OTA
{
private:
  bool isOTAEnabled = false;

public:
  void SetupOTA(const char *OTAPassword, uint16_t OTAServerPort)
  {
    log_i("Setting up OTA updates");

    if (OTAPassword == nullptr)
    {
      log_e("THE PASSWORD IS REQUIRED, [[ABORTING]]");
      return;
    }
    ArduinoOTA.setPort(OTAServerPort);
    isOTAEnabled = true;

    ArduinoOTA
        .onStart([]()
                 {
                String type;
                if (ArduinoOTA.getCommand() == U_FLASH)
                    type = "sketch";
                else // U_SPIFFS
                    type = "filesystem"; })
        .onEnd([]()
               { log_i("OTA updated finished successfully!"); })
        .onProgress([](unsigned int progress, unsigned int total)
                    { log_i("Progress: %u%%\r", (progress / (total / 100))); })
        .onError([](ota_error_t error)
                 {
                log_e("Error[%u]: ", error);
                if (error == OTA_AUTH_ERROR) log_e("Auth Failed");
                else if (error == OTA_BEGIN_ERROR) log_e("Begin Failed");
                else if (error == OTA_CONNECT_ERROR) log_e("Connect Failed");
                else if (error == OTA_RECEIVE_ERROR) log_e("Receive Failed");
                else if (error == OTA_END_ERROR) log_e("End Failed"); });
    log_i("Starting up basic OTA server");
    log_i("OTA will be live for 30s, after which it will be disabled until restart");
    ArduinoOTA.begin();
  }

  void HandleOTAUpdate()
  {
    if (isOTAEnabled)
    {
      ArduinoOTA.handle();
    }
  }
};
