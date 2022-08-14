#include "WifiHandler.hpp"
#include <vector>

WiFiHandler::WiFiHandler(ProjectConfig *configManager, StateManager<WiFiState_e> *stateManager) : conf(new wifi_config_t),
                                                                                                  configManager(configManager),
                                                                                                  stateManager(stateManager) {}

WiFiHandler::~WiFiHandler() {}

void WiFiHandler::setupWifi()
{
  if (ENABLE_ADHOC || stateManager->getCurrentState() == WiFiState_e::WiFiState_ADHOC)
  {
    this->setUpADHOC();
    return;
  }
  log_i("Initializing connection to wifi");
  stateManager->setState(WiFiState_e::WiFiState_Connecting);

  std::vector<ProjectConfig::WiFiConfig_t> *networks = configManager->getWifiConfigs();
  int connection_timeout = 30000; // 30 seconds

  int count = 0;
  unsigned long currentMillis = millis();
  unsigned long _previousMillis = currentMillis;

  for (auto networkIterator = networks->begin(); networkIterator != networks->end(); ++networkIterator)
  {
    log_i("Trying to connect to the %s network", networkIterator->ssid);

    WiFi.begin(networkIterator->ssid, networkIterator->password);
    count++;

    if (!WiFi.isConnected())
      log_i("\n\rCould not connect to %s, trying another network\n\r", networkIterator->ssid);
    else
    {
      log_i("\n\rSuccessfully connected to %s\n\r", networkIterator->ssid);
      stateManager->setState(WiFiState_e::WiFiState_Connected);
      return;
    }

    while (WiFi.status() != WL_CONNECTED)
    {
      stateManager->setState(ProgramStates::DeviceStates::WiFiState_e::WiFiState_Connecting);
      currentMillis = millis();
      Serial.print(".");
      delay(300);
      if (((currentMillis - _previousMillis) >= connection_timeout) && count >= networks->size())
      {
        log_i("[INFO]: WiFi connection timed out.\n");
        // we've tried all saved networks, none worked, let's error out
        log_e("Could not connect to any of the save networks, check your Wifi credentials");
        stateManager->setState(WiFiState_e::WiFiState_Error);
        this->setUpADHOC();
        log_w("Setting up adhoc");
        log_w("Please set your WiFi credentials and reboot the device");
        stateManager->setState(WiFiState_e::WiFiState_ADHOC);
        return;
      }
    }
  }
}

void WiFiHandler::adhoc(const char *ssid, const char *password, uint8_t channel)
{
  log_i("[INFO]: Setting Access Point...\n");

  log_i("[INFO]: Configuring access point...\n");
  WiFi.mode(WIFI_AP);

  Serial.printf("\r\nStarting AP. \r\nAP IP address: ");
  IPAddress IP = WiFi.softAPIP();
  Serial.printf("[INFO]: AP IP address: %s.\r\n", IP.toString().c_str());

  // You can remove the password parameter if you want the AP to be open.
  WiFi.softAP(ssid, password, channel, 0, 3); // AP mode with password

  WiFi.setTxPower(WIFI_POWER_11dBm);
  stateManager->setState(WiFiState_e::WiFiState_ADHOC);
}

void WiFiHandler::setUpADHOC()
{
  size_t ssidLen = strlen((char *)conf->ap.ssid);
  size_t passwordLen = strlen((char *)conf->ap.password);
  char ap_ssid[ssidLen + 1];
  char ap_password[passwordLen + 1];
  auto ret = esp_wifi_get_config(WIFI_IF_STA, &*conf);
  if (ret == ESP_OK)
  {
    memcpy(ap_ssid, conf->ap.ssid, ssidLen);
    memcpy(ap_password, conf->ap.password, passwordLen);

    ap_ssid[ssidLen] = '\0';         // Null-terminate the string
    ap_password[passwordLen] = '\0'; // Null-terminate the string
    return;
  }
  
  if (ssidLen == 0)
  {
    strcpy(ap_ssid, WIFI_SSID);
    strcpy(ap_password, WIFI_PASSWORD);
    conf->ap.channel = ADHOC_CHANNEL;
  }

  this->adhoc(ap_ssid, ap_password, conf->ap.channel);
}

// we can't assign wifiManager.resetSettings(); to reset, somehow it gets called straight away.
/**
 * @brief Resets the wifi settings to the chosen settings.
 *
 * @param value - value to store - string.
 * @param location - location to store the value. byte array - conf
 */
void WiFiHandler::setWiFiConf(const char *value, uint8_t *location, wifi_config_t *conf)
{
  assert(conf != nullptr);
#if defined(ESP32)
  if (WiFiGenericClass::getMode() != WIFI_MODE_NULL)
  {
    esp_wifi_get_config(WIFI_IF_STA, conf);

    memset(location, 0, sizeof(location));
    for (int i = 0; i < sizeof(value) / sizeof(value[0]) && i < sizeof(location); i++)
      location[i] = value[i];

    esp_wifi_set_config(WIFI_IF_STA, conf);
  }
#endif
}