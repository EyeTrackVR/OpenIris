#include <network/mdns/mdns_manager.hpp>
#include <openiris.hpp>

/**
 * @brief ProjectConfig object
 * @brief This is the main configuration object for the project
 * @param name The name of the project config partition
 * @param mdnsName The mDNS hostname to use
 */
ProjectConfig deviceConfig("openiris", MDNS_HOSTNAME);
ConfigHandler configHandler(deviceConfig);
OpenIrisConfig projectConfig(deviceConfig);

#ifdef CONFIG_CAMERA_MODULE_ESP32S3_XIAO_SENSE
LEDManager ledManager(LED_BUILTIN);
#else
LEDManager ledManager(33);
#endif  // ESP32S3_XIAO_SENSE

#ifndef SIM_ENABLED
CameraHandler cameraHandler(projectConfig);
#endif  // SIM_ENABLED

#ifndef ETVR_EYE_TRACKER_USB_API
WiFiHandler network(deviceConfig, WIFI_SSID, WIFI_PASSWORD, WIFI_CHANNEL);
MDNSHandler mdnsHandler(deviceConfig,
                        "_openiristracker",
                        "etvr_tracker",
                        "_tcp",
                        "stream_port",
                        "80");
RestAPI apiServer(deviceConfig, projectConfig);
#ifdef SIM_ENABLED
#else
StreamServer streamServer;
#endif  // SIM_ENABLED

void etvr_eye_tracker_web_init() {
  log_d("[SETUP]: Starting Network Handler");
  // deviceConfig.attach(network);
  log_d("[SETUP]: Checking ADHOC Settings");
  // FIXME: This is not working
  // network._enable_adhoc = ENABLE_ADHOC;
  deviceConfig.attach(mdnsHandler);
  log_d("[SETUP]: Starting WiFi Handler");
  network.begin();
  log_d("[SETUP]: Starting MDNS Handler");
  mdnsHandler.begin();
  log_d("[SETUP]: Starting API Server");
  apiServer.begin();
#endif  // ETVR_EYE_TRACKER_WEB_API
}

void setup() {
  setCpuFrequencyMhz(240);
  Serial.begin(115200);
  Logo::printASCII();
  //  Serial.flush();
  ledManager.begin();

#ifndef SIM_ENABLED
  projectConfig.attach(cameraHandler);
#endif  // SIM_ENABLED
  //* Register the config handler
  deviceConfig.attach(configHandler);
  //* Register the project config
  deviceConfig.registerUserConfig(&projectConfig);
  //* Load Config from memory
  configHandler.begin();

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
#endif  // ETVR_EYE_TRACKER_USB_API
}
