#include <Arduino.h>
#include <io/Improv/improvHandler.hpp>
#include <io/LEDManager/LEDManager.hpp>
#include <io/camera/cameraHandler.hpp>
#include <network/WifiHandler/WifiHandler.hpp>
#include <network/api/webserverHandler.hpp>
#include <network/mDNS/MDNSManager.hpp>
#include <network/stream/streamServer.hpp>

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

LEDManager ledManager(33, &ledStateManager);
ImprovHandler improvHandler(&deviceConfig, &wifiStateManager,&ledStateManager);

#ifndef SIM_ENABLED
CameraHandler cameraHandler(&deviceConfig, &ledStateManager);
#endif  // SIM_ENABLED
WiFiHandler wifiHandler(&deviceConfig,
                        &wifiStateManager,
                        &ledStateManager,
                        WIFI_SSID,
                        WIFI_PASSWORD,
                        WIFI_CHANNEL);
APIServer apiServer(CONTROL_SERVER_PORT,
                    &deviceConfig,
                    &cameraHandler,
                    &wifiStateManager,
                    "/control");
MDNSHandler mdnsHandler(&mdnsStateManager, &deviceConfig);

#ifndef SIM_ENABLED
StreamServer streamServer(STREAM_SERVER_PORT, &wifiStateManager);
#endif  // SIM_ENABLED

void setup() {
  setCpuFrequencyMhz(240);  // set to 240mhz for performance boost
  Serial.begin(115200);
  // Serial.setDebugOutput(DEBUG_MODE);
  // Serial.println("Free Heap: " + String(ESP.getFreeHeap()));
  Logo::printASCII();
  Serial.flush();
  // Serial.println("Free Heap: " + String(ESP.getFreeHeap()));
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

  /* mdnsStateManager.setState(MDNSState_e::MDNSState_Starting);
      switch (mdnsStateManager.getCurrentState())
      {
      case MDNSState_e::MDNSState_Starting:
              break;
      case MDNSState_e::MDNSState_Error:
              break;
      case MDNSState_e::MDNSState_QueryComplete:
              mdnsHandler.startMDNS();
              break;
      default:
              break;
      } */

  switch (wifiStateManager.getCurrentState()) {
    case WiFiState_e::WiFiState_Disconnected: {
      //! TODO: Implement
      break;
    }
    case WiFiState_e::WiFiState_ADHOC: {
#ifndef SIM_ENABLED
      streamServer.startStreamServer();
      log_d("[SETUP]: Starting Stream Server");
#endif  // SIM_ENABLED
      apiServer.begin();
      log_d("[SETUP]: Starting API Server");
      break;
    }
    case WiFiState_e::WiFiState_Connected: {
#ifndef SIM_ENABLED
      streamServer.startStreamServer();
      log_d("[SETUP]: Starting Stream Server");
#endif  // SIM_ENABLED
      apiServer.begin();
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
#if ENABLE_OTA
  ota.begin();
#endif  // ENABLE_OTA
}

void loop() {
#if ENABLE_OTA
  ota.handleOTAUpdate();
#endif  // ENABLE_OTA
  ledManager.handleLED();
  //* Handle Improv
  improvHandler.loop();
}
