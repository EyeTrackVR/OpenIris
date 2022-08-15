#include "WifiHandler.hpp"
#include <vector>

WiFiHandler::WiFiHandler(ProjectConfig *configManager, StateManager<WiFiState_e> *stateManager) : configManager(configManager),
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

		WiFi.begin(networkIterator->ssid.c_str(), networkIterator->password.c_str());
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
				this->iniSTA();
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

/*
* *
*/
void WiFiHandler::setUpADHOC()
{
	log_i("[INFO]: Setting Access Point...\n");
	size_t ssidLen = strlen(configManager->getAPWifiConfig()->ssid.c_str());
	size_t passwordLen = strlen(configManager->getAPWifiConfig()->password.c_str());
	char ssid[ssidLen + 1];
	char password[passwordLen + 1];
	uint8_t channel = configManager->getAPWifiConfig()->channel;
	if (ssidLen > 0 || passwordLen > 0)
	{
		strcpy(ssid, configManager->getAPWifiConfig()->ssid.c_str());
		strcpy(password, configManager->getAPWifiConfig()->password.c_str());
		channel = configManager->getAPWifiConfig()->channel;
	}
	else
	{
		strcpy(ssid, WIFI_AP_SSID);
		strcpy(password, WIFI_AP_PASSWORD);
		channel = ADHOC_CHANNEL;
	}

	this->adhoc(ssid, password, channel);

	log_i("[INFO]: Configuring access point...\n");
	log_d("[DEBUG]: ssid: %s\n", ssid);
	log_d("[DEBUG]: password: %s\n", password);
	log_d("[DEBUG]: channel: %d\n", channel);
}

void WiFiHandler::iniSTA()
{
	log_i("[INFO]: Setting up station...\n");
	int connection_timeout = 30000; // 30 seconds
	unsigned long currentMillis = millis();
	unsigned long _previousMillis = currentMillis;

	log_i("Trying to connect to the %s network", WIFI_SSID);

	WiFi.begin(WIFI_SSID, WIFI_PASSWORD, WIFI_CHANNEL);

	if (!WiFi.isConnected())
		log_i("\n\rCould not connect to %s, please try another network\n\r", WIFI_SSID);
	else
	{
		log_i("\n\rSuccessfully connected to %s\n\r", WIFI_SSID);
		stateManager->setState(WiFiState_e::WiFiState_Connected);
		return;
	}

	while (WiFi.status() != WL_CONNECTED)
	{
		stateManager->setState(ProgramStates::DeviceStates::WiFiState_e::WiFiState_Connecting);
		currentMillis = millis();
		Serial.print(".");
		delay(300);
		if ((currentMillis - _previousMillis) >= connection_timeout)
		{
			log_i("[INFO]: WiFi connection timed out.\n");
			// we've tried all saved networks, none worked, let's error out
			log_e("Could not connect to any of the save networks, check your Wifi credentials");
			stateManager->setState(WiFiState_e::WiFiState_Error);
			this->iniSTA();
			log_w("Setting up adhoc");
			log_w("Please set your WiFi credentials and reboot the device");
			stateManager->setState(WiFiState_e::WiFiState_ADHOC);
			return;
		}
	}
}