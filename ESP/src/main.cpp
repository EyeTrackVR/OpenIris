#include "AsyncUDP.h"

#include <Arduino.h>

#include <data/config/project_config.hpp>
#include <io/LEDManager/LEDManager.hpp>
#include <io/camera/cameraHandler.hpp>
#include <logo/logo.hpp>

#ifndef ETVR_EYE_TRACKER_USB_API
#include <network/api/webserverHandler.hpp>
#include <network/mDNS/MDNSManager.hpp>
#include <network/UDPStream/UDPStreamHandler.h>
//#include <network/WebSocketHandler/WebSocketHandler.h>
//#include <network/UDPStream/UDPStreamHandler.h>
//#include <network/stream/streamServer.hpp>
#include <network/wifihandler/wifihandler.hpp>
#else
#include <usb/etvr_eye_tracker_usb.hpp>
#endif  // ETVR_EYE_TRACKER_WEB_API

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

#ifndef ETVR_EYE_TRACKER_USB_API
WiFiHandler wifiHandler(deviceConfig, WIFI_SSID, WIFI_PASSWORD, WIFI_CHANNEL);
MDNSHandler mdnsHandler(deviceConfig);

#ifndef SIM_ENABLED
int port = 81;
#else
int port = 80;
#endif

AsyncWebServer server(port);
APIServer apiServer(server, deviceConfig, cameraHandler, "/control");

#ifndef SIM_ENABLED
UDPStreamHandler udpStreamHandler(deviceConfig);
//WebSocketHandler webSocketHandler(server);
//StreamServer streamServer;
#endif  // SIM_ENABLED

void etvr_eye_tracker_web_init() {
  deviceConfig.attach(mdnsHandler);
  //deviceConfig.attach(wifiHandler);
  wifiHandler._enable_adhoc = ENABLE_ADHOC;
  wifiHandler.begin();
  mdnsHandler.startMDNS();

  switch (wifiStateManager.getCurrentState()) {
    case WiFiState_e::WiFiState_Disconnected: {
      udpStreamHandler.stop();
      break;
    }
    case WiFiState_e::WiFiState_ADHOC: {
#ifndef SIM_ENABLED
//      log_d("[SETUP]: Starting Stream Websocket");
//      webSocketHandler.begin();
//      streamServer.startStreamServer();
#endif  // SIM_ENABLED
      log_d("[SETUP]: Starting API Server");
      apiServer.setup();
      break;
    }
    case WiFiState_e::WiFiState_Connected: {
#ifndef SIM_ENABLED
//      log_d("[SETUP]: Starting Stream Websocket");
//      webSocketHandler.begin();
//      streamServer.startStreamServer();
#endif  // SIM_ENABLED
      log_d("[SETUP]: Starting API Server");
      apiServer.setup();
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
  deviceConfig.attach(udpStreamHandler);
#endif  // SIM_ENABLED

  deviceConfig.initConfig();
  deviceConfig.load();

#ifndef ETVR_EYE_TRACKER_USB_API
  etvr_eye_tracker_web_init();
#else   // ETVR_EYE_TRACKER_WEB_API
  WiFi.disconnect(true);
  etvr_eye_tracker_usb_init();
#endif  // ETVR_EYE_TRACKER_WEB_API
}

void loop() {
  ledManager.handleLED();
#ifdef ETVR_EYE_TRACKER_USB_API
  etvr_eye_tracker_usb_loop();
#else
//  webSocketHandler.stream();
udpStreamHandler.stream();
#endif  // ETVR_EYE_TRACKER_USB_API
}
