#include <Arduino.h>
#include <io/LEDManager/LEDManager.hpp>
#include <io/camera/cameraHandler.hpp>
#include <network/WifiHandler/WifiHandler.hpp>
#include <network/api/webserverHandler.hpp>
#include <network/mDNS/MDNSManager.hpp>
#include <network/stream/streamServer.hpp>

//! TODO: Setup OTA enabled state to be controllable by API if enabled at
//! compile time
#if ENABLE_OTA
#include <network/OTA/OTA.hpp>
#endif  // ENABLE_OTA
#include <data/config/project_config.hpp>
#include <logo/logo.hpp>

int STREAM_SERVER_PORT = 80;
int CONTROL_SERVER_PORT = 81;
/**
 * @brief ProjectConfig object
 * @brief This is the main configuration object for the project
 * @param name The name of the project config partition
 * @param mdnsName The mDNS hostname to use
 */
ProjectConfig deviceConfig("openiris", MDNS_HOSTNAME);

#if ENABLE_OTA
OTA ota(&deviceConfig);
#endif  // ENABLE_OTA
LEDManager ledManager(33);
AsyncWebServer webserver = AsyncWebServer(CONTROL_SERVER_PORT);

#ifndef SIM_ENABLED
CameraHandler cameraHandler(&deviceConfig, &ledStateManager);
#endif  // SIM_ENABLED
WiFiHandler wifiHandler(&deviceConfig,
                        &wifiStateManager,
                        WIFI_SSID,
                        WIFI_PASSWORD,
                        WIFI_CHANNEL);

#ifndef SIM_ENABLED
APIServer apiServer(&webserver,
                    &deviceConfig,
                    &cameraHandler,
                    &wifiStateManager,
                    "/control");
#else
APIServer apiServer(&webserver,
                    &deviceConfig,
                    NULL,
                    &wifiStateManager,
                    "/control");
#endif  // SIM_ENABLED
MDNSHandler mdnsHandler(&mdnsStateManager, &deviceConfig);

#ifndef SIM_ENABLED
StreamServer streamServer(STREAM_SERVER_PORT, &wifiStateManager);
#endif  // SIM_ENABLED

void setup() {
  setCpuFrequencyMhz(240);  // set to 240mhz for performance boost
  Serial.begin(115200);

  Logo::printASCII();
  Serial.flush();

  ledManager.begin();
#ifndef SIM_ENABLED
  deviceConfig.attach(&cameraHandler);
#endif  // SIM_ENABLED
  deviceConfig.attach(&mdnsHandler);
  deviceConfig.initConfig();
  deviceConfig.load();
  wifiHandler._enable_adhoc = ENABLE_ADHOC;
  wifiHandler.setupWifi();
  mdnsHandler.startMDNS();

  ota.setup(&webserver);

  switch (wifiStateManager.getCurrentState()) {
    case WiFiState_e::WiFiState_Disconnected: {
      //! TODO: Implement
      break;
    }
    case WiFiState_e::WiFiState_Disconnecting: {
      //! TODO: Implement
      break;
    }
    case WiFiState_e::WiFiState_ADHOC: {
#ifndef SIM_ENABLED
      streamServer.startStreamServer();
      log_d("[SETUP]: Starting Stream Server");
#endif  // SIM_ENABLED
      apiServer.setup();
      webserver.begin();
      log_d("[SETUP]: Starting API Server");
      break;
    }
    case WiFiState_e::WiFiState_Connected: {
#ifndef SIM_ENABLED
      streamServer.startStreamServer();
      log_d("[SETUP]: Starting Stream Server");
#endif  // SIM_ENABLED
      apiServer.setup();
      webserver.begin();
      log_d("[SETUP]: Starting API Server");
      break;
    }
    case WiFiState_e::WiFiState_Connecting: {
      //! TODO: Implement
      break;
    }
    case WiFiState_e::WiFiState_Error: {
      //! TODO: Implement
      break;
    }
  }
}

void loop() {
  ledManager.handleLED(&ledStateManager);
}
