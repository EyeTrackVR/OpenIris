#include <Arduino.h>
#include <io/Improv/improvHandler.hpp>
#include <io/LEDManager/LEDManager.hpp>
#include <io/camera/cameraHandler.hpp>
#include <network/api/webserverHandler.hpp>
#include <network/mDNS/MDNSManager.hpp>
#include <network/stream/streamServer.hpp>
#include <network/wifihandler/wifiHandler.hpp>

#if ENABLE_OTA
#include <network/OTA/OTA.hpp>
#endif  // ENABLE_OTA
#include <data/config/project_config.hpp>
#include <logo/logo.hpp>

/**
 * @brief ProjectConfig object
 * @brief This is the main configuration object for the project
 * @param name The name of the project config partition
 * @param mdnsName The mDNS hostname to use
 */
ProjectConfig deviceConfig("openiris", MDNS_HOSTNAME);

#if ENABLE_OTA
OTA ota(deviceConfig);
#endif  // ENABLE_OTA

LEDManager ledManager(33);

#ifndef SIM_ENABLED
CameraHandler cameraHandler(deviceConfig);
#endif  // SIM_ENABLED
WiFiHandler wifiHandler(deviceConfig, WIFI_SSID, WIFI_PASSWORD, WIFI_CHANNEL);

ImprovHandler improvHandler(deviceConfig);

#ifndef SIM_ENABLED
APIServer apiServer(deviceConfig, cameraHandler, "/control", 81);
#else
APIServer apiServer(deviceConfig, "/control", 80);
#endif  // SIM_ENABLED

MDNSHandler mdnsHandler(deviceConfig);

#ifndef SIM_ENABLED
StreamServer streamServer;
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
  deviceConfig.attach(cameraHandler);
#endif  // SIM_ENABLED
  deviceConfig.attach(mdnsHandler);
  deviceConfig.attach(wifiHandler);
  deviceConfig.initConfig();
  deviceConfig.load();
  wifiHandler._enable_adhoc = ENABLE_ADHOC;
  wifiHandler.begin();
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
  improvHandler.loop();
  ledManager.handleLED();
#if ENABLE_OTA
  ota.handleOTAUpdate();
#endif  // ENABLE_OTA
  //* Handle Improv
}
