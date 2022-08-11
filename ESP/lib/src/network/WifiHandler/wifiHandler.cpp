#include "WifiHandler.hpp"
#include <vector>

WiFiHandler::WiFiHandler(ProjectConfig *configManager, StateManager<ProgramStates::DeviceStates::WiFiState_e> *stateManager) : conf(new wifi_config_t),
                                                                                                                               configManager(configManager),
                                                                                                                               stateManager(stateManager) {}

WiFiHandler::~WiFiHandler() {}

void WiFiHandler::setupWifi()
{
  if (ENABLE_ADHOC)
  {
    this->setUpADHOC();
    return;
  }
  log_i("Initializing connection to wifi");
  stateManager->setState((ProgramStates::DeviceStates::WiFiState_e::WiFiState_Connecting));

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
      stateManager->setState(ProgramStates::DeviceStates::WiFiState_e::WiFiState_Connected);
      return;
    }
  }

  // we've tried all saved networks, none worked, let's error out
  log_e("Could not connect to any of the save networks, check your Wifi credentials");
  stateManager->setState(ProgramStates::DeviceStates::WiFiState_e::WiFiState_Error);
}

void WiFiHandler::setUpADHOC()
{
  log_i("[INFO]: Setting Access Point...\n");

  log_i("[INFO]: Configuring access point...\n");
  WiFi.mode(WIFI_AP);

  // You can remove the password parameter if you want the AP to be open.
  Serial.printf("\r\nStarting AP. \r\nAP IP address: ");
  IPAddress IP = WiFi.softAPIP();
  Serial.printf("[INFO]: AP IP address: %s.\r\n", IP.toString().c_str());

  WiFi.softAP(WIFI_SSID, WIFI_PASSWORD, ADHOC_CHANNEL, 0, 3); // AP mode with password

  WiFi.setTxPower(WIFI_POWER_11dBm);
  stateManager->setState((ProgramStates::DeviceStates::WiFiState_e::WiFiState_ADHOC));
}

// we can't assign wifiManager.resetSettings(); to reset, somehow it gets called straight away.
/**
 * @brief Resets the wifi settings to the chosen settings.
 *
 * @param value - value to store - string.
 * @param location - location to store the value. byte array - conf
 */
void WiFiHandler::setWiFiConf(const char *value, uint8_t *location)
{
#if defined(ESP32)
  if (WiFiGenericClass::getMode() != WIFI_MODE_NULL)
  {
    esp_wifi_get_config(WIFI_IF_STA, &*conf);

    memset(location, 0, sizeof(location));
    for (int i = 0; i < sizeof(value) / sizeof(value[0]) && i < sizeof(location); i++)
      location[i] = value[i];

    esp_wifi_set_config(WIFI_IF_STA, &*conf);
  }
#endif
}