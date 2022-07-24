#include "WifiHandler.hpp"
#include <vector>

void WiFiHandler::setupWifi(StateManager<ProgramStates::DeviceStates::WiFiState_e> *stateManager, ProjectConfig *configManager)
{
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
