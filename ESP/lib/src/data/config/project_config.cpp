#include "project_config.hpp"

ProjectConfig::ProjectConfig(const std::string &name) : _name(std::move(name)), _already_loaded(false) {}

ProjectConfig::~ProjectConfig() {}

/**
 *@brief Initializes the structures with blank data to prevent empty memory sectors and nullptr errors.
 *@brief This is to be called in setup() before loading the config.
 */
void ProjectConfig::initConfig()
{
	if (_name.empty())
	{
		log_e("Config name is null\n");
		_name = "openiris";
	}

	bool success = begin(_name.c_str());

	log_i("Config name: %s", _name.c_str());
	log_i("Config loaded: %s", success ? "true" : "false");

	this->config.device = {
		_name,
		"12345678",
		3232,
		false,
		false,
		false,
		"",
		"",
		"",
	};

	this->config.ap_network = {
		"",
		"",
		1,
	};
}

void ProjectConfig::save()
{
	log_d("Saving project config");
	deviceConfigSave();
	cameraConfigSave();
	wifiConfigSave();
	end();
}

void ProjectConfig::wifiConfigSave()
{
	log_d("Saving wifi config");

	/* WiFi Config */
	putInt("networkCount", this->config.networks.size());

	std::string name = "name";
	std::string ssid = "ssid";
	std::string password = "pass";
	std::string channel = "channel";
	for (int i = 0; i < this->config.networks.size(); i++)
	{
		char buffer[2];
		std::string iter_str = Helpers::itoa(i, buffer, 10);

		name.append(iter_str);
		ssid.append(iter_str);
		password.append(iter_str);
		channel.append(iter_str);

		putString(name.c_str(), this->config.networks[i].name.c_str());
		putString(ssid.c_str(), this->config.networks[i].ssid.c_str());
		putString(password.c_str(), this->config.networks[i].password.c_str());
		putInt(channel.c_str(), this->config.networks[i].channel);
	}

	/* AP Config */
	putString("apSSID", this->config.ap_network.ssid.c_str());
	putString("apPass", this->config.ap_network.password.c_str());
	putUInt("apChannel", this->config.ap_network.channel);

	log_i("Project config saved and system is rebooting");
	ESP.restart();
}

void ProjectConfig::deviceConfigSave()
{
	/* Device Config */
	putString("deviceName", this->config.device.name.c_str());
	putString("OTAPassword", this->config.device.OTAPassword.c_str());
	putInt("OTAPort", this->config.device.OTAPort);
	//! No need to save the JSON strings or bools, they are generated and used on the fly
}

void ProjectConfig::cameraConfigSave()
{
	/* Camera Config */
	putInt("vflip", this->config.camera.vflip);
	putInt("framesize", this->config.camera.framesize);
	putInt("href", this->config.camera.href);
	putInt("quality", this->config.camera.quality);
}

bool ProjectConfig::reset()
{
	log_w("Resetting project config");
	return clear();
}

void ProjectConfig::load()
{
	log_d("Loading project config");
	if (this->_already_loaded)
	{
		log_w("Project config already loaded");
		return;
	}

	/* Device Config */
	this->config.device.name = getString("deviceName", "openiris").c_str();
	this->config.device.OTAPassword = getString("OTAPassword", "12345678").c_str();
	this->config.device.OTAPort = getInt("OTAPort", 3232);
	//! No need to load the JSON strings or bools, they are generated and used on the fly

	/* WiFi Config */
	int networkCount = getInt("networkCount", 0);
	std::string name = "name";
	std::string ssid = "ssid";
	std::string password = "pass";
	std::string channel = "channel";
	for (int i = 0; i < networkCount; i++)
	{
		char buffer[2];
		std::string iter_str = Helpers::itoa(i, buffer, 10);

		name.append(iter_str);
		ssid.append(iter_str);
		password.append(iter_str);
		channel.append(iter_str);

		const std::string &temp_1 = getString(name.c_str()).c_str();
		const std::string &temp_2 = getString(ssid.c_str()).c_str();
		const std::string &temp_3 = getString(password.c_str()).c_str();
		uint8_t temp_4 = getUInt(channel.c_str());

		//! push_back creates a copy of the object, so we need to use emplace_back
		this->config.networks.emplace_back(
				temp_1,
				temp_2,
				temp_3,
				temp_4,
				false); // false because the networks we store in the config are the ones we want the esp to connect to, rather than host as AP
	}

	/* AP Config */
	this->config.ap_network.ssid = getString("apSSID", "openiris").c_str();
	this->config.ap_network.password = getString("apPass", "12345678").c_str();
	this->config.ap_network.channel = getUInt("apChannel", 1);

	this->_already_loaded = true;
	this->notify(ObserverEvent::configLoaded);
}

//**********************************************************************************************************************
//*
//!                                                DeviceConfig
//*
//**********************************************************************************************************************
void ProjectConfig::setDeviceConfig(const std::string &name, const std::string &OTAPassword, int *OTAPort, bool shouldNotify)
{
	log_d("Updating device config");
	this->config.device.name.assign(name);
	this->config.device.OTAPassword.assign(OTAPassword);
	this->config.device.OTAPort = *OTAPort;

	if (shouldNotify)
		this->notify(ObserverEvent::deviceConfigUpdated);
}

void ProjectConfig::setCameraConfig(uint8_t *vflip, uint8_t *framesize, uint8_t *href, uint8_t *quality, bool shouldNotify)
{
	log_d("Updating camera config");
	this->config.camera.vflip = *vflip;
	this->config.camera.framesize = *framesize;
	this->config.camera.href = *href;
	this->config.camera.quality = *quality;

	log_d("Updating Camera config");
	if (shouldNotify)
		this->notify(ObserverEvent::cameraConfigUpdated);
}

void ProjectConfig::setWifiConfig(const std::string &networkName, const std::string &ssid, const std::string &password, uint8_t *channel, bool adhoc, bool shouldNotify)
{
	WiFiConfig_t *networkToUpdate = nullptr;

	// we store the ADHOC flag as false because the networks we store in the config are the ones we want the esp to connect to, rather than host as AP, and here we're just updating them
	size_t size = this->config.networks.size();
	if (size > 0)
	{
		for (int i = 0; i < size; i++)
		{
			if (strcmp(this->config.networks[i].name.c_str(), networkName.c_str()) == 0)
				networkToUpdate = &this->config.networks[i];

			//! push_back creates a copy of the object, so we need to use emplace_back
			if (networkToUpdate != nullptr)
			{
				this->config.networks.emplace_back(
						networkName,
						ssid,
						password,
						*channel,
						false);
			}
			log_d("Updating wifi config");
		}
	}
	else
	{
		//! push_back creates a copy of the object, so we need to use emplace_back
		this->config.networks.emplace_back(
				networkName,
				ssid,
				password,
				*channel,
				false);
		networkToUpdate = &this->config.networks[0];
	}

	if (shouldNotify)
		this->notify(ObserverEvent::networksConfigUpdated);
}

void ProjectConfig::setAPWifiConfig(const std::string &ssid, const std::string &password, uint8_t *channel, bool adhoc, bool shouldNotify)
{
	this->config.ap_network.ssid.assign(ssid);
	this->config.ap_network.password.assign(password);
	this->config.ap_network.channel = *channel;
	this->config.ap_network.adhoc = adhoc;

	log_d("Updating access point config");
	if (shouldNotify)
		this->notify(ObserverEvent::networksConfigUpdated);
}