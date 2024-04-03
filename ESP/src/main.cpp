#include <openiris.hpp>
/**
 * @brief ProjectConfig object
 * @brief This is the main configuration object for the project
 * @param name The name of the project config partition
 * @param mdnsName The mDNS hostname to use
 */
ProjectConfig deviceConfig("openiris", MDNS_HOSTNAME);
CommandManager commandManager(&deviceConfig);
SerialManager serialManager(&commandManager, deviceConfig);

#ifdef CONFIG_CAMERA_MODULE_ESP32S3_XIAO_SENSE
LEDManager ledManager(LED_BUILTIN);
#else
LEDManager ledManager(33);
#endif  // ESP32S3_XIAO_SENSE

#ifndef SIM_ENABLED
CameraHandler cameraHandler(deviceConfig);
#endif  // SIM_ENABLED

#ifndef ETVR_EYE_TRACKER_USB_API
WiFiHandler wifiHandler(deviceConfig, WIFI_SSID, WIFI_PASSWORD, WIFI_CHANNEL, ENABLE_ADHOC);
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

auto device_config = deviceConfig.getDeviceConfig();
auto networks = deviceConfig.getWifiConfigs();

Serial.println("[DEBUG] PRINTING LAST RECEIVED SERIAL MESSAGE");
Serial.println(device_config.SerialJSONData.c_str());

Serial.println("[DEBUG] TRYING TO DECODE THIS MESSAGE LIVE");

JsonDocument doc;
DeserializationError deserializationError = deserializeJson(doc, device_config.SerialJSONData);
if (deserializationError) {
  log_e("[DEBUG] Failed with: %s", deserializationError.c_str());
} else {
  Serial.println("[DEBUG] DECODING SUCCESS, PRINTING WHAT DATA WE GOT");

  for(JsonVariant commandData: doc["commands"].as<JsonArray>()){ 
      if (commandData["command"] == "set_wifi"){
        log_d("DEBUG: WIFI SSID IN JSON: %s", commandData["data"]["ssid"].as<String>().c_str());
        log_d("DEBUG: WIFI PASSWORD IN JSON: %s", commandData["data"]["password"].as<String>().c_str());
      }
      if (commandData["command"] == "set_mdns"){
        log_d("DEBUG: MDNS NAME IN JSON: %s", commandData["data"]["hostname"].as<String>().c_str());
      }
  }
}

Serial.println("[DEBUG] PRINTING ALL SAVED NETWORKS");
for (auto& network : networks) {
  log_d("[DEBUG] SAVED NETWORKP: %s %s", network.ssid.c_str(), network.password.c_str());
}

Serial.println("[DEBUG] END");
}

void loop() {
  ledManager.handleLED();
  serialManager.run();
}
