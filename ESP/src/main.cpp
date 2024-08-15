#include <openiris.hpp>
/**
 * @brief ProjectConfig object
 * @brief This is the main configuration object for the project
 * @param name The name of the project config partition
 * @param mdnsName The mDNS hostname to use
 */
ProjectConfig deviceConfig("openiris", MDNS_HOSTNAME);
CommandManager commandManager(deviceConfig);
SerialManager serialManager(&commandManager);

#ifdef CONFIG_CAMERA_MODULE_ESP32S3_XIAO_SENSE
LEDManager ledManager(LED_BUILTIN);
#elif CONFIG_CAMERA_MODULE_SWROOM_BABBLE_S3
LEDManager ledManager(38);
#else
LEDManager ledManager(33);
#endif

#ifndef SIM_ENABLED
CameraHandler cameraHandler(deviceConfig);
#endif  // SIM_ENABLED

#ifndef ETVR_EYE_TRACKER_USB_API
WiFiHandler wifiHandler(deviceConfig,
                        WIFI_SSID,
                        WIFI_PASSWORD,
                        WIFI_CHANNEL,
                        ENABLE_ADHOC);
MDNSHandler mdnsHandler(deviceConfig);

APIServer apiServer(deviceConfig, cameraHandler, "/control");

#ifndef SIM_ENABLED
StreamServer streamServer;
#endif  // SIM_ENABLED

void etvr_eye_tracker_web_init() {
  log_d("[SETUP]: Starting Network Handler");
  deviceConfig.attach(mdnsHandler);
  log_d("[SETUP]: Starting WiFi Handler");
  wifiHandler.begin();
  log_d("[SETUP]: Starting MDNS Handler");
  mdnsHandler.startMDNS();

  auto wifiState = wifiStateManager.getCurrentState();
#ifndef SIM_ENABLED
  if (wifiState == WiFiState_e::WiFiState_Connected ||
      wifiState == WiFiState_e::WiFiState_ADHOC) {
    {
      log_d("[SETUP]: Starting Stream Server");
      auto result = streamServer.startTCPStreamServer();
      // streamServer.startStreamServer();
      // auto result = streamServer.startUDPStreamServer();

      log_d("[SETUP]: Stream Server state: %s",
            result ? "Connected" : "Failed to connect");
      log_d("[SETUP]: Starting API Server");
      apiServer.setup();
    }
#endif  // SIM_ENABLED
  }
}
#endif  // ETVR_EYE_TRACKER_WEB_API

void setup() {
  setCpuFrequencyMhz(240);
  Serial.begin(115200);
  Logo::printASCII();
  ledManager.begin();

#ifndef SIM_ENABLED
  deviceConfig.attach(cameraHandler);
#endif  // SIM_ENABLED
  deviceConfig.load();
  serialManager.init();

#ifndef ETVR_EYE_TRACKER_USB_API
  etvr_eye_tracker_web_init();
#else   // ETVR_EYE_TRACKER_WEB_API
  WiFi.disconnect(true);
#endif  // ETVR_EYE_TRACKER_WEB_API
}

void loop() {
#ifndef ETVR_EYE_TRACKER_USB_API
  streamServer.sendTCPFrame();
#endif
  // streamServer.sendUDPFrame();
  ledManager.handleLED();
  serialManager.run();
}
