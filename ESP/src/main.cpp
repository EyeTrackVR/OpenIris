#include <openiris.hpp>
/**
 * @brief ProjectConfig object
 * @brief This is the main configuration object for the project
 * @param name The name of the project config partition
 * @param mdnsName The mDNS hostname to use
 */
ProjectConfig deviceConfig("openiris", MDNS_HOSTNAME);
CommandManager commandManager(&deviceConfig);
SerialManager serialManager(&commandManager);

#ifdef CONFIG_CAMERA_MODULE_ESP32S3_XIAO_SENSE
LEDManager ledManager(LED_BUILTIN);

#elif CONFIG_CAMERA_MODULE_SWROOM_BABBLE_S3
LEDManager ledManager(38);

#else
LEDManager ledManager(33);
#endif  // ESP32S3_XIAO_SENSE

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
#ifdef SIM_ENABLED
APIServer apiServer(deviceConfig, wifiStateManager, "/control");
#else
APIServer apiServer(deviceConfig, cameraHandler, "/control");
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
  if (wifiState == WiFiState_e::WiFiState_ADHOC ||
      wifiState == WiFiState_e::WiFiState_Connected) {
#ifndef SIM_ENABLED
    log_d("[SETUP]: Starting Stream Server");
    streamServer.startStreamServer();
#endif  // SIM_ENABLED
    log_d("[SETUP]: Starting API Server");
    apiServer.setup();
  }
}
#endif  // ETVR_EYE_TRACKER_WEB_API

void setup() {
  setCpuFrequencyMhz(240);
  Serial.begin(115200);
  Logo::printASCII();
  ledManager.begin();

#ifdef CONFIG_CAMERA_MODULE_SWROOM_BABBLE_S3  // Set IR emitter strength to
                                              // 100%.
  const int ledPin = 1;  // Replace this with a command endpoint eventually.
  const int freq = 5000;
  const int ledChannel = 0;
  const int resolution = 8;
  const int dutyCycle = 255;
  ledcSetup(ledChannel, freq, resolution);
  ledcAttachPin(1, ledChannel);
  ledcWrite(ledChannel, dutyCycle);
#endif

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
  ledManager.handleLED();
  serialManager.run();
}
