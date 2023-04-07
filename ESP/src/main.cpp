#include <etvr_system.hpp>

#include <Arduino.h>

#include <data/config/project_config.hpp>
#include <io/LEDManager/LEDManager.hpp>
#include <io/camera/cameraHandler.hpp>
#include <logo/logo.hpp>

#ifdef ETVR_EYE_TRACKER_WEB_API
#include <network/api/webserverHandler.hpp>
#include <network/mDNS/MDNSManager.hpp>
#include <network/stream/streamServer.hpp>
#include <network/wifihandler/wifihandler.hpp>
#endif  // ETVR_EYE_TRACKER_WEB_API

#ifdef ETVR_EYE_TRACKER_USB_API
#include <usb/etvr_eye_tracker_usb.hpp>
#endif  // ETVR_EYE_TRACKER_USB_API

/**
 * @brief ProjectConfig object
 * @brief This is the main configuration object for the project
 * @param name The name of the project config partition
 * @param mdnsName The mDNS hostname to use
 */
ProjectConfig deviceConfig("openiris", MDNS_HOSTNAME);

LEDManager ledManager(33);

#ifndef SIM_ENABLED
CameraHandler cameraHandler(deviceConfig);
#endif  // SIM_ENABLED

#ifdef ETVR_EYE_TRACKER_WEB_API
WiFiHandler wifiHandler(deviceConfig, WIFI_SSID, WIFI_PASSWORD, WIFI_CHANNEL);
MDNSHandler mdnsHandler(deviceConfig);
#ifdef SIM_ENABLED
APIServer apiServer(deviceConfig, wifiStateManager, "/control");
#else
APIServer apiServer(deviceConfig, cameraHandler, "/control");
StreamServer streamServer;
#endif  // SIM_ENABLED

void etvr_eye_tracker_web_init() {
  wifiHandler._enable_adhoc = ENABLE_ADHOC;
  wifiHandler.begin();
  mdnsHandler.startMDNS();

  switch (wifiStateManager.getCurrentState()) {
    case WiFiState_e::WiFiState_Disconnected: {
      //! TODO: Implement
      break;
    }
    case WiFiState_e::WiFiState_ADHOC: {
#ifndef SIM_ENABLED
      log_d("[SETUP]: Starting Stream Server");
      streamServer.startStreamServer();
#endif  // SIM_ENABLED
      log_d("[SETUP]: Starting API Server");
      apiServer.setup();
      break;
    }
    case WiFiState_e::WiFiState_Connected: {
#ifndef SIM_ENABLED
      log_d("[SETUP]: Starting Stream Server");
      streamServer.startStreamServer();
#endif  // SIM_ENABLED
      log_d("[SETUP]: Starting API Server");
      apiServer.setup();
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
#endif  // ETVR_EYE_TRACKER_WEB_API

void setup() {
  setCpuFrequencyMhz(240);
  Serial.begin(115200);
  Logo::printASCII();
  Serial.flush();
  ledManager.begin();

#ifndef SIM_ENABLED
  deviceConfig.attach(cameraHandler);
#endif  // SIM_ENABLED

#ifdef ETVR_EYE_TRACKER_WEB_API
  deviceConfig.attach(mdnsHandler);
  deviceConfig.attach(wifiHandler);
#endif  // ETVR_EYE_TRACKER_WEB_API

  deviceConfig.initConfig();
  deviceConfig.load();

#ifdef ETVR_EYE_TRACKER_WEB_API
  etvr_eye_tracker_web_init();
#endif  // ETVR_EYE_TRACKER_WEB_API

#ifdef ETVR_EYE_TRACKER_USB_API
  etvr_eye_tracker_usb_init();
#endif  // ETVR_EYE_TRACKER_USB_API
}

void loop() {
  ledManager.handleLED();
#ifdef ETVR_EYE_TRACKER_USB_API
  etvr_eye_tracker_usb_loop();
#endif  // ETVR_EYE_TRACKER_USB_API
}
