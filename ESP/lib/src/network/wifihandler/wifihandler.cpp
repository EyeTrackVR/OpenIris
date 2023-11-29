#include "wifihandler.hpp"
#include <WiFi.h>
#include "data/StateManager/StateManager.hpp"
#include "data/utilities/helpers.hpp"

WiFiHandler::WiFiHandler(ProjectConfig& configManager,
                         const std::string& ssid,
                         const std::string& password,
                         uint8_t channel)
    : configManager(configManager),
      ssid(std::move(ssid)),
      password(std::move(password)),
      channel(channel),
      power(0),
      _enable_adhoc(false) {}

WiFiHandler::~WiFiHandler() {}

void WiFiHandler::begin() {
  log_i("Starting WiFi Handler \n\r");
  if (this->_enable_adhoc ||
      wifiStateManager.getCurrentState() == WiFiState_e::WiFiState_ADHOC) {
    log_d("ADHOC is enabled, setting up ADHOC network \n\r");
    this->setUpADHOC();
    return;
  }

  log_d(
      "ADHOC is disabled, setting up STA network and checking transmission "
      "power \n\r");
  auto txpower = configManager.getWiFiTxPowerConfig();
  log_d("Setting Wifi Power to: %d", txpower.power);
  log_d("Setting WiFi sleep mode to NONE \n\r");
  WiFi.setSleep(false);

  log_i("Initializing connection to wifi \n\r");
  wifiStateManager.setState(WiFiState_e::WiFiState_Connecting);

  auto networks = configManager.getWifiConfigs();

  if (networks.empty()) {
    log_i("No networks found in config, trying the default one \n\r");
    if (this->iniSTA(this->ssid, this->password, this->channel,
                     (wifi_power_t)txpower.power)) {
      return;
    }
    log_i(
        "Could not connect to the hardcoded network, setting up ADHOC "
        "network \n\r");
    this->setUpADHOC();
    return;
  }

  for (auto& network : networks) {
    log_i("Trying to connect to network: %s \n\r", network.ssid.c_str());
    if (this->iniSTA(network.ssid, network.password, network.channel,
                     (wifi_power_t)network.power)) {
      return;
    }
  }

  // at this point, we've tried every network, let's just setup adhoc
  log_i(
      "We've gone through every network, each timed out. Trying to connect "
      "to hardcoded network: %s \n\r",
      this->ssid.c_str());
  if (this->iniSTA(this->ssid, this->password, this->channel,
                   (wifi_power_t)txpower.power)) {
    log_i("Successfully connected to the hardcoded network. \n\r");
    return;
  }

  log_i(
      "Could not connect to the hardcoded network, setting up adhoc. "
      "\n\r");
  this->setUpADHOC();
}

void WiFiHandler::adhoc(const std::string& ssid,
                        uint8_t channel,
                        const std::string& password) {
  wifiStateManager.setState(WiFiState_e::WiFiState_ADHOC);

  log_i("\n[INFO]: Configuring access point...\n");
  WiFi.mode(WIFI_AP);
  WiFi.setSleep(WIFI_PS_NONE);
  Serial.printf("\r\nStarting AP. \r\n");
  IPAddress IP = WiFi.softAPIP();
  Serial.printf("[INFO]: AP IP address: %s.\r\n", IP.toString().c_str());
  // You can remove the password parameter if you want the AP to be open.
  ProjectConfig::WiFiTxPower_t txpower = configManager.getWiFiTxPowerConfig();
  WiFi.softAP(ssid.c_str(), password.c_str(),
              channel);  // AP mode with password
  WiFi.setTxPower((wifi_power_t)txpower.power);
}

void WiFiHandler::setUpADHOC() {
  log_i("\n[INFO]: Setting Up Access Point...\n");
  size_t ssidLen = configManager.getAPWifiConfig().ssid.length();
  size_t passwordLen = configManager.getAPWifiConfig().password.length();
  if (ssidLen <= 0) {
    log_i("\n[INFO]: Configuring access point with default values\n");
    this->adhoc(WIFI_AP_SSID, WIFI_AP_CHANNEL, WIFI_AP_PASSWORD);
    return;
  }

  if (passwordLen <= 0) {
    log_i("\n[INFO]: Configuring access point without a password\n");
    this->adhoc(configManager.getAPWifiConfig().ssid,
                configManager.getAPWifiConfig().channel);
    return;
  }

  this->adhoc(configManager.getAPWifiConfig().ssid,
              configManager.getAPWifiConfig().channel,
              configManager.getAPWifiConfig().password);

  log_i("\n[INFO]: Configuring access point...\n");
  log_d("\n[DEBUG]: ssid: %s\n", configManager.getAPWifiConfig().ssid.c_str());
  log_d("\n[DEBUG]: password: %s\n",
        configManager.getAPWifiConfig().password.c_str());
  log_d("\n[DEBUG]: channel: %d\n", configManager.getAPWifiConfig().channel);
}

bool WiFiHandler::iniSTA(const std::string& ssid,
                         const std::string& password,
                         uint8_t channel,
                         wifi_power_t power) {
  unsigned long currentMillis = millis();
  unsigned long startingMillis = currentMillis;
  int connectionTimeout = 45000;  // 30 seconds
  int progress = 0;

  wifiStateManager.setState(WiFiState_e::WiFiState_Connecting);
  log_i("Trying to connect to: %s \n\r", ssid.c_str());
  auto mdnsConfig = configManager.getMDNSConfig();

  log_d("Setting hostname %s \n\r");
  WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE,
              INADDR_NONE);  // need to call before setting hostname
  WiFi.setHostname(mdnsConfig.hostname.c_str());
  WiFi.begin(ssid.c_str(), password.c_str(), channel);
  WiFi.setTxPower(power);
  log_d("Waiting for WiFi to connect... \n\r");
  while (WiFi.status() != WL_CONNECTED) {
    progress++;
    currentMillis = millis();
    log_i(".");
    log_d("Progress: %d \n\r", progress);
    /*  Helpers::update_progress_bar(progress, 100);
     delay(301); */
    if ((currentMillis - startingMillis) >= connectionTimeout) {
      wifiStateManager.setState(WiFiState_e::WiFiState_Error);
      log_e("Connection to: %s TIMEOUT \n\r", ssid.c_str());
      return false;
    }
  }
  wifiStateManager.setState(WiFiState_e::WiFiState_Connected);
  log_i("Successfully connected to %s \n\r", ssid.c_str());
  log_i("Setting TX power to: %d \n\r", (uint8_t)power);
  return true;
}

void WiFiHandler::update(ConfigState_e event) {
  switch (event) {
    case ConfigState_e::networksConfigUpdated:
      this->begin();
      break;
    default:
      break;
  }
}

std::string WiFiHandler::getName() {
  return "WiFiHandler";
}
