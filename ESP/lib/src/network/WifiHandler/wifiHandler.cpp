#include "WifiHandler.hpp"

void WiFiHandler::setupWifi(const char *ssid, const char *password, StateManager<ProgramStates::DeviceStates::WiFiState_e> *stateManager)
{
  log_d("Initializing connection to wifi");

  WiFi.mode(WIFI_STA);
  WiFi.setSleep(WIFI_PS_NONE);
  WiFi.begin(ssid, password);

  log_d("connecting");
  int time_spent_connecting = 0;
  int connection_timeout = 6400;
  int wifi_status = WiFi.status();

  while (time_spent_connecting < connection_timeout || wifi_status != WL_CONNECTED)
  {
    wifi_status = WiFi.status();
    Serial.print(".");
    stateManager->setState((ProgramStates::DeviceStates::WiFiState_e::WiFiState_Connecting));
    time_spent_connecting += 1600;
    delay(1600);
  }

  if (wifi_status == WL_CONNECTED)
  {
    stateManager->setState(ProgramStates::DeviceStates::WiFiState_e::WiFiState_Connected);
    delay(1600);
    log_i("\n\rWiFi connected\n\r");
    log_i("ESP will be streaming under 'http://%s:80/\r\n", WiFi.localIP().toString().c_str());
    log_i("ESP will be accepting commands under 'http://%s:81/control\r\n", WiFi.localIP().toString().c_str());
  }
  else
  {
    stateManager->setState(ProgramStates::DeviceStates::WiFiState_e::WiFiState_Error);
    return;
  }
}
