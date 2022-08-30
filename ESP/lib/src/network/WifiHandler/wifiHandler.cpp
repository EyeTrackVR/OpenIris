#include "WifiHandler.hpp"
#include <vector>

WiFiHandler::WiFiHandler(ProjectConfig *configManager,
						 StateManager<WiFiState_e> *stateManager,
						 const std::string &ssid,
						 const std::string &password,
						 uint8_t channel) : configManager(configManager),
											stateManager(stateManager),
											ssid(ssid),
											password(password),
											channel(channel),
											_enable_adhoc(false) {}

WiFiHandler::~WiFiHandler() {}

void WiFiHandler::setupWifi()
{
	if (this->_enable_adhoc || stateManager->getCurrentState() == WiFiState_e::WiFiState_ADHOC)
	{
		this->setUpADHOC();
		return;
	}

	log_i("Initializing connection to wifi");
	stateManager->setState(WiFiState_e::WiFiState_Connecting);

	std::vector<ProjectConfig::WiFiConfig_t> *networks = configManager->getWifiConfigs();

	// check size of networks
	log_i("Found %d networks", networks->size());

	/* if (networks->empty())
	{
		log_e("No networks found in config");
		this->iniSTA();
		stateManager->setState(WiFiState_e::WiFiState_Error);
		return;
	} */

	if (networks->size() == 0)
	{
		log_e("No networks found in config");
		stateManager->setState(WiFiState_e::WiFiState_Error);
		this->iniSTA();
		return;
	}

	int connection_timeout = 30000; // 30 seconds

	int count = 0;
	unsigned long currentMillis = millis();
	unsigned long _previousMillis = currentMillis;
	int progress = 0;

	WiFi.mode(WIFI_STA);
	WiFi.setSleep(WIFI_PS_NONE);
	for (auto networkIterator = networks->begin(); networkIterator != networks->end(); ++networkIterator)
	{
		log_i("Trying to connect to the %s network", networkIterator->ssid.c_str());
		WiFi.begin(networkIterator->ssid.c_str(), networkIterator->password.c_str());
		count++;

		while (WiFi.status() != WL_CONNECTED)
		{
			progress++;
			stateManager->setState(ProgramStates::DeviceStates::WiFiState_e::WiFiState_Connecting);
			currentMillis = millis();
			Helpers::update_progress_bar(progress, 100);
			delay(301);
			if (((currentMillis - _previousMillis) >= connection_timeout) && (count <= networks->size()))
			{
				log_i("\n[INFO]: WiFi connection timed out.\n");
				// we've tried all saved networks, none worked, let's error out
				log_e("\nCould not connect to any of the saved networks, check your Wifi credentials");
				stateManager->setState(WiFiState_e::WiFiState_Disconnected);
				log_i("\n[INFO]: Attempting to connect to hardcoded network");
				this->iniSTA();
				return;
			}
		}
		log_i("\n\rSuccessfully connected to %s\n\r", networkIterator->ssid);
		stateManager->setState(WiFiState_e::WiFiState_Connected);
	}
}

void WiFiHandler::adhoc(const char *ssid, const char *password, uint8_t channel)
{
	stateManager->setState(WiFiState_e::WiFiState_ADHOC);
	log_i("\n[INFO]: Setting Access Point...\n");
	log_i("\n[INFO]: Configuring access point...\n");
	WiFi.mode(WIFI_AP);
	WiFi.setSleep(WIFI_PS_NONE);
	Serial.printf("\r\nStarting AP. \r\nAP IP address: ");
	IPAddress IP = WiFi.softAPIP();
	Serial.printf("[INFO]: AP IP address: %s.\r\n", IP.toString().c_str());
	// You can remove the password parameter if you want the AP to be open.
	WiFi.softAP(ssid, password, channel); // AP mode with password
	WiFi.setTxPower(WIFI_POWER_11dBm);
}

/*
 * *
 */
void WiFiHandler::setUpADHOC()
{
	log_i("\n[INFO]: Setting Access Point...\n");
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
		strcpy(ssid, "OpenIris");
		strcpy(password, "12345678");
		channel = 1;
	}
	this->adhoc(ssid, password, channel);
	log_i("\n[INFO]: Configuring access point...\n");
	log_d("\n[DEBUG]: ssid: %s\n", ssid);
	log_d("\n[DEBUG]: password: %s\n", password);
	log_d("\n[DEBUG]: channel: %d\n", channel);
}

void WiFiHandler::iniSTA()
{
	log_i("\n[INFO]: Setting up station...\n");
	int connection_timeout = 30000; // 30 seconds
	unsigned long currentMillis = millis();
	unsigned long _previousMillis = currentMillis;
	int progress = 0;
	log_i("Trying to connect to the %s network", this->ssid.c_str());
	//  check size of networks
	if (this->ssid.size() == 0)
	{
		log_e("No networks passed into the constructor");
		stateManager->setState(WiFiState_e::WiFiState_Error);
		this->setUpADHOC();
		return;
	}

	WiFi.mode(WIFI_STA);
	WiFi.setSleep(WIFI_PS_NONE);

	WiFi.begin(this->ssid.c_str(), this->password.c_str(), this->channel);
	while (WiFi.status() != WL_CONNECTED)
	{
		stateManager->setState(ProgramStates::DeviceStates::WiFiState_e::WiFiState_Connecting);
		currentMillis = millis();
		Helpers::update_progress_bar(progress, 100);
		delay(301);
		if ((currentMillis - _previousMillis) >= connection_timeout)
		{
			log_i("\n[INFO]: WiFi connection timed out.\n");
			// we've tried all saved networks, none worked, let's error out
			log_e("Could not connect to any of the save networks, check your Wifi credentials");
			stateManager->setState(WiFiState_e::WiFiState_Error);
			this->setUpADHOC();
			log_w("Setting up adhoc mode");
			log_w("Please use adhoc mode and the app to set your WiFi credentials and reboot the device");
			return;
		}
	}
	log_i("\n\rSuccessfully connected to %s\n\r", this->ssid.c_str());
	stateManager->setState(WiFiState_e::WiFiState_Connected);
}
