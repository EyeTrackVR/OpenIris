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
  int connection_timeout = 3000;

  for (auto networkIterator = networks->begin(); networkIterator != networks->end(); ++networkIterator)
  {
    log_i("Trying to connect to the %s network", networkIterator->ssid);

    int timeSpentConnecting = 0;
    WiFi.begin(networkIterator->ssid, networkIterator->password);
    int wifi_status = WiFi.status();

    while (timeSpentConnecting < connection_timeout || wifi_status != WL_CONNECTED)
    {
      wifi_status = WiFi.status();
      log_i(".");
      timeSpentConnecting += 300;
      delay(300);
    }

    if (!WiFi.isConnected())
      log_i("\n\rCould not connect to %s, trying another network\n\r", networkIterator->ssid);
    else
    {
      log_i("\n\rSuccessfully connected to %s\n\r", networkIterator->ssid);
      stateManager->setState(WiFiState_e::WiFiState_Connected);
      return;
    }
  }

  // we've tried all saved networks, none worked, let's error out
  log_e("Could not connect to any of the save networks, check your Wifi credentials");
  stateManager->setState(WiFiState_e::WiFiState_Error);
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
  unsigned int ap_ssid_length = sizeof(conf->ap.ssid);
  unsigned int ap_password_length = sizeof(conf->ap.password);

  char ap_ssid[ap_ssid_length + 1];
  char ap_password[ap_ssid_length + 1];
  memcpy(ap_ssid, conf->ap.ssid, ap_ssid_length);
  memcpy(ap_password, conf->ap.password, ap_password_length);

  ap_ssid[ap_ssid_length] = '\0';         // Null-terminate the string
  ap_password[ap_password_length] = '\0'; // Null-terminate the string
  if (ap_ssid[0] == '\0' || NULL)
  {
    log_i("[INFO]: No SSID or password has been set.\n");
    log_i("[INFO]: USing the default value.\r\n");
    strcpy(ap_ssid, WIFI_SSID);
  }

  if (ap_password[0] == '\0' || NULL)
  {
    log_i("[INFO]: No Password has been set.\n");
    log_i("[INFO]: Using the default value.\r\n");
    strcpy(ap_password, WIFI_PASSWORD);
  }

  if (conf->ap.channel == 0 || NULL)
  {
    log_i("[INFO]: No channel has been set.\n");
    log_i("[INFO]: Using the default value.\r\n");
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