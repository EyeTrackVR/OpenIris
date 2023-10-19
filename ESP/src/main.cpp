#include <network/mdns/mdns_manager.hpp>
#include <openiris.hpp>

/**
 * @brief ProjectConfig object
 * @brief This is the main configuration object for the project
 * @param name The name of the project config partition
 * @param mdnsName The mDNS hostname to use
 */
ConfigHandler configHandler("openiris", MDNS_HOSTNAME);
OpenIrisConfig projectConfig(configHandler.config);

API api(configHandler.config, projectConfig);

#ifdef CONFIG_CAMERA_MODULE_ESP32S3_XIAO_SENSE
LEDManager ledManager(LED_BUILTIN);
#else
LEDManager ledManager(33);
#endif  // ESP32S3_XIAO_SENSE

#ifndef ETVR_EYE_TRACKER_USB_API
WiFiHandler network(configHandler.config,
                    WIFI_SSID,
                    WIFI_PASSWORD,
                    WIFI_CHANNEL);
MDNSHandler mdnsHandler(configHandler.config,
                        "_openiristracker",
                        "etvr_tracker",
                        "_tcp",
                        "stream_port",
                        "80");
RestAPI apiServer(api);
#ifdef SIM_ENABLED
#else
StreamServer streamServer;
#endif  // SIM_ENABLED

void web_init() {
  log_d("[SETUP]: Starting Network Handler");
  // configHandler.config.attach(network);
  log_d("[SETUP]: Checking ADHOC Settings");
  // FIXME: This is not working
  // network._enable_adhoc = ENABLE_ADHOC;
  configHandler.config.attach(mdnsHandler);
  log_d("[SETUP]: Starting WiFi Handler");
  network.begin();
  log_d("[SETUP]: Starting MDNS Handler");
  mdnsHandler.begin();
  log_d("[SETUP]: Starting API Server");
  apiServer.begin();
  streamServer.begin();
}
#endif  // ETVR_EYE_TRACKER_WEB_API

void setup() {
  Serial.begin(115200);
  Logo::printASCII();
  setCpuFrequencyMhz(240);
  ledManager.begin();

#ifndef SIM_ENABLED
  projectConfig.attach(api.cameraHandler);
#endif  // SIM_ENABLED
  //* Register the config handler
  configHandler.config.attach(configHandler);
  //* Register the project config
  configHandler.config.registerUserConfig(&projectConfig);
  //* Load Config from memory
  configHandler.begin();

#ifndef ETVR_EYE_TRACKER_USB_API
  web_init();
#else   // ETVR_EYE_TRACKER_WEB_API
  WiFi.disconnect(true);
  usb_init();
#endif  // ETVR_EYE_TRACKER_WEB_API
}

void loop() {
  ledManager.handleLED();
#ifdef ETVR_EYE_TRACKER_USB_API
  usb_loop();
#endif  // ETVR_EYE_TRACKER_USB_API
}
