#include "WifiHandler.hpp"

WiFiHandler::WiFiHandler(ProjectConfig* configManager,
                         StateManager<WiFiState_e>* stateManager,
                         const std::string& ssid,
                         const std::string& password,
                         uint8_t channel)
    : configManager(configManager),
      stateManager(stateManager),
      ssid(ssid),
      password(password),
      channel(channel),
      power(0),
      _enable_adhoc(false) {}

WiFiHandler::~WiFiHandler() {}

void WiFiHandler::setupWifi() {
  if (this->_enable_adhoc ||
      stateManager->getCurrentState() == WiFiState_e::WiFiState_ADHOC) {
    this->setUpADHOC();
    return;
  }
  ProjectConfig::WiFiTxPower_t* txpower = configManager->getWiFiTxPowerConfig();
  WiFi.mode(WIFI_STA);
  WiFi.setSleep(WIFI_PS_NONE);

  log_i("Initializing connection to wifi \n\r");
  stateManager->setState(WiFiState_e::WiFiState_Connecting);

  std::vector<ProjectConfig::WiFiConfig_t>* networks =
      configManager->getWifiConfigs();

  if (networks->empty()) {
    log_i("No networks found in config, trying the default one \n\r");
    if (this->iniSTA(this->ssid, this->password, this->channel,
                     (wifi_power_t)txpower->power)) {
      return;
    } else {
      log_i(
          "Could not connect to the hardcoded network, setting up ADHOC "
          "network \n\r");
      this->setUpADHOC();
      return;
    }
  }

  for (auto networkIterator = networks->begin();
       networkIterator != networks->end(); ++networkIterator) {
    if (this->iniSTA(networkIterator->ssid, networkIterator->password,
                     networkIterator->channel,
                     (wifi_power_t)networkIterator->power)) {
      return;
    }
  }

  // at this point, we've tried every network, let's just setup adhoc
  log_i(
      "We've gone through every network, each timed out. Trying to connect "
      "to hardcoded network: %s \n\r",
      this->ssid.c_str());
  if (this->iniSTA(this->ssid, this->password, this->channel,
                   (wifi_power_t)txpower->power)) {
    log_i("Successfully connected to the hardcoded network. \n\r");
  } else {
    log_i(
        "Could not connect to the hardcoded network, setting up adhoc. "
        "\n\r");
    this->setUpADHOC();
  }
}

void WiFiHandler::adhoc(const std::string& ssid,
                        uint8_t channel,
                        const std::string& password) {
  stateManager->setState(WiFiState_e::WiFiState_ADHOC);

  log_i("\n[INFO]: Configuring access point...\n");
  WiFi.mode(WIFI_AP);
  WiFi.setSleep(WIFI_PS_NONE);
  Serial.printf("\r\nStarting AP. \r\n");
  IPAddress IP = WiFi.softAPIP();
  Serial.printf("[INFO]: AP IP address: %s.\r\n", IP.toString().c_str());
  // You can remove the password parameter if you want the AP to be open.
  ProjectConfig::WiFiTxPower_t* txpower = configManager->getWiFiTxPowerConfig();
  WiFi.softAP(ssid.c_str(), password.c_str(),
              channel);  // AP mode with password
  WiFi.setTxPower((wifi_power_t)txpower->power);
}

void WiFiHandler::setUpADHOC() {
  log_i("\n[INFO]: Setting Up Access Point...\n");
  size_t ssidLen = configManager->getAPWifiConfig()->ssid.length();
  size_t passwordLen = configManager->getAPWifiConfig()->password.length();
  if (ssidLen <= 0) {
    log_i("\n[INFO]: Configuring access point with default values\n");
    this->adhoc(WIFI_AP_SSID, WIFI_AP_CHANNEL, WIFI_AP_PASSWORD);
    return;
  }

  if (passwordLen <= 0) {
    log_i("\n[INFO]: Configuring access point without a password\n");
    this->adhoc(configManager->getAPWifiConfig()->ssid,
                configManager->getAPWifiConfig()->channel);
    return;
  }

  this->adhoc(configManager->getAPWifiConfig()->ssid,
              configManager->getAPWifiConfig()->channel,
              configManager->getAPWifiConfig()->password);

  log_i("\n[INFO]: Configuring access point...\n");
  log_d("\n[DEBUG]: ssid: %s\n",
        configManager->getAPWifiConfig()->ssid.c_str());
  log_d("\n[DEBUG]: password: %s\n",
        configManager->getAPWifiConfig()->password.c_str());
  log_d("\n[DEBUG]: channel: %d\n", configManager->getAPWifiConfig()->channel);
}

bool WiFiHandler::iniSTA(const std::string& ssid,
                         const std::string& password,
                         uint8_t channel,
                         wifi_power_t power) {
  unsigned long currentMillis = millis();
  unsigned long startingMillis = currentMillis;
  int connectionTimeout = 30000;  // 30 seconds
  int progress = 0;

  stateManager->setState(WiFiState_e::WiFiState_Connecting);
  log_i("Trying to connect to: %s \n\r", ssid.c_str());

  auto mdnsConfig = configManager->getMDNSConfig();
  WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE,
              INADDR_NONE);  // need to call before setting hostname
  WiFi.setHostname(mdnsConfig->hostname.c_str());
  WiFi.begin(ssid.c_str(), password.c_str(), channel);
  while (WiFi.status() != WL_CONNECTED) {
    progress++;
    currentMillis = millis();
    Helpers::update_progress_bar(progress, 100);
    delay(301);
    if ((currentMillis - startingMillis) >= connectionTimeout) {
      stateManager->setState(WiFiState_e::WiFiState_Error);
      log_e("Connection to: %s TIMEOUT \n\r", ssid.c_str());
      return false;
    }
  }
  stateManager->setState(WiFiState_e::WiFiState_Connected);
  log_i("Successfully connected to %s \n\r", ssid.c_str());
  log_i("Setting TX power to: %d \n\r", (uint8_t)power);
  WiFi.setTxPower(power);
  return true;
}
