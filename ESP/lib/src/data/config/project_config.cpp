#include "project_config.hpp"

ProjectConfig::ProjectConfig() : _already_loaded(false) {}

ProjectConfig::~ProjectConfig() {}

/**
 *@brief Initializes the structures with blank data to prevent empty memory sectors and nullptr errors.
 *@brief This is to be called in setup() before loading the config.
 */
void ProjectConfig::initConfig()
{
	begin("projectConf");
	this->config.device = {
		"eyetrackvr",
		"",
		3232,
		false,
		false,
		false,
		"",
		"",
		""};

	this->config.camera = {
		0,
		0,
		0,
		0,
	};

	this->config.networks = {
		{
			"",
			"",
			"",
			0,
			false,
		},
	};

	this->config.ap_network = {
		"",
		"",
		0,
		false,
	};
}

void ProjectConfig::load()
{
	log_d("Loading project config");
	if (this->_already_loaded)
	{
		log_w("Project config already loaded");
		return;
	}

	size_t configLen = getBytesLength("config");
	if (configLen == 0)
	{
		log_e("Project config not found - Generating config and restarting");
		save();
		delay(1000);
		ESP.restart();
		return;
	}
	
	log_d("Project config found - Config length: %d", configLen);

	char buff[configLen];
	getBytes("config", buff, configLen);

	for (int i = 0; i < configLen; i++)
		Serial.printf("%02X ", buff[i]);

	this->_already_loaded = true;
	this->notify(ObserverEvent::configLoaded);
}

void ProjectConfig::save()
{
	log_d("Saving project config");

	TrackerConfig_t *tracker_config = (TrackerConfig_t *)&this->config;
	putBytes("config", tracker_config, 3 * sizeof(TrackerConfig_t));

	log_i("Project config saved and system is rebooting");
	delay(20000);
	ESP.restart();
}

void ProjectConfig::reset()
{
	log_w("Resetting project config");
	this->clear();
}

//**********************************************************************************************************************
//*
//*                                                DeviceConfig
//*
//**********************************************************************************************************************
void ProjectConfig::setDeviceConfig(const char *name, const char *OTAPassword, int *OTAPort, bool shouldNotify)
{
	log_d("Updating device config");
	this->config.device = {
		(char *)name,
		(char *)OTAPassword,
		*OTAPort,
	};
	if (shouldNotify)
	{
		this->notify(ObserverEvent::deviceConfigUpdated);
	}
}

void ProjectConfig::setCameraConfig(uint8_t *vflip, uint8_t *framesize, uint8_t *href, uint8_t *quality, bool shouldNotify)
{
	this->config.camera = {
		*vflip,
		*framesize,
		*href,
		*quality,
	};

	log_d("Updating camera config");
	if (shouldNotify)
	{
		this->notify(ObserverEvent::cameraConfigUpdated);
	}
}

void ProjectConfig::setWifiConfig(const char *networkName, const char *ssid, const char *password, uint8_t *channel, bool adhoc, bool shouldNotify)
{
	WiFiConfig_t *networkToUpdate = nullptr;

	for (int i = 0; i < this->config.networks.size(); i++)
	{
		if (strcmp(this->config.networks[i].name.c_str(), networkName) == 0)
			networkToUpdate = &this->config.networks[i];
	}

	if (networkToUpdate != nullptr)
	{
		this->config.networks = {
			{
				(char *)networkName,
				(char *)ssid,
				(char *)password,
				*channel,
				adhoc,
			},
		};
		if (shouldNotify)
			this->notify(ObserverEvent::networksConfigUpdated);
	}
	log_d("Updating wifi config");
}

void ProjectConfig::setAPWifiConfig(const char *ssid, const char *password, uint8_t *channel, bool adhoc, bool shouldNotify)
{
	this->config.ap_network = {
		(char *)ssid,
		(char *)password,
		*channel,
		adhoc,
	};

	log_d("Updating access point config");
	if (shouldNotify)
	{
		this->notify(ObserverEvent::networksConfigUpdated);
	}
}