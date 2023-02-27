#include "project_config.hpp"

ProjectConfig::ProjectConfig(const std::string& name,
                             const std::string& mdnsName)
    : _name(std::move(name)),
      _mdnsName(std::move(mdnsName)),
      _already_loaded(false) {}

ProjectConfig::~ProjectConfig() {}

/**
 *@brief Initializes the structures with blank data to prevent empty memory
 *sectors and nullptr errors.
 *@brief This is to be called in setup() before loading the config.
 */
void ProjectConfig::initConfig() {
  if (_name.empty()) {
    log_e("Config name is null\n");
    _name = "openiris";
  }

  bool success = begin(_name.c_str());

  log_i("Config name: %s", _name.c_str());
  log_i("Config loaded: %s", success ? "true" : "false");

  /*
  * If the config is not loaded,
  * we need to initialize the config with default data
  ! Do not initialize the WiFiConfig_t struct here,
  ! as it will create a blank network which breaks the WiFiManager
   */
  this->config.device = {
      "12345678",
      3232,
  };

  if (_mdnsName.empty()) {
    log_e("MDNS name is null\n Autoassigning name to 'openiristracker'");
    _mdnsName = "openiristracker";
  }
  this->config.mdns = {
      _mdnsName,
      "openiristracker",
  };

  log_i("MDNS name: %s", _mdnsName.c_str());

  this->config.ap_network = {
      "",
      "",
      1,
      false,
  };

  this->config.camera = {
      .vflip = 0,
      .href = 0,
      .framesize = 4,
      .quality = 7,
      .brightness = 2,
  };
}

void ProjectConfig::save() {
  log_d("Saving project config");
  deviceConfigSave();
  mdnsConfigSave();
  cameraConfigSave();
  wifiConfigSave();
  wifiTxPowerConfigSave();
  end();  // we call end() here to close the connection to the NVS partition, we
          // only do this because we call ESP.restart() next.
  OpenIrisTasks::ScheduleRestart(2000);
}

void ProjectConfig::wifiConfigSave() {
  log_d("Saving wifi config");

  /* WiFi Config */
  putInt("networkCount", this->config.networks.size());

  std::string name = "name";
  std::string ssid = "ssid";
  std::string password = "pass";
  std::string channel = "channel";
  std::string power = "power";
  for (int i = 0; i < this->config.networks.size(); i++) {
    char buffer[2];
    std::string iter_str = Helpers::itoa(i, buffer, 10);

    name.append(iter_str);
    ssid.append(iter_str);
    password.append(iter_str);
    channel.append(iter_str);
    power.append(iter_str);

    putString(name.c_str(), this->config.networks[i].name.c_str());
    putString(ssid.c_str(), this->config.networks[i].ssid.c_str());
    putString(password.c_str(), this->config.networks[i].password.c_str());
    putUInt(channel.c_str(), this->config.networks[i].channel);
    putUInt(power.c_str(), this->config.networks[i].power);
  }

  /* AP Config */
  putString("apSSID", this->config.ap_network.ssid.c_str());
  putString("apPass", this->config.ap_network.password.c_str());
  putUInt("apChannel", this->config.ap_network.channel);

  log_i("Project config saved and system is rebooting");
}

void ProjectConfig::deviceConfigSave() {
  /* Device Config */
  putString("OTAPassword", this->config.device.OTAPassword.c_str());
  putInt("OTAPort", this->config.device.OTAPort);
}

void ProjectConfig::mdnsConfigSave() {
  /* Device Config */
  putString("hostname", this->config.mdns.hostname.c_str());
  putString("service", this->config.mdns.service.c_str());
}

void ProjectConfig::wifiTxPowerConfigSave() {
  /* Device Config */
  putInt("power", this->config.txpower.power);
}

void ProjectConfig::cameraConfigSave() {
  /* Camera Config */
  putInt("vflip", this->config.camera.vflip);
  putInt("href", this->config.camera.href);
  putInt("framesize", this->config.camera.framesize);
  putInt("quality", this->config.camera.quality);
  putInt("brightness", this->config.camera.brightness);
}

bool ProjectConfig::reset() {
  log_w("Resetting project config");
  return clear();
}

void ProjectConfig::load() {
  log_d("Loading project config");
  if (this->_already_loaded) {
    log_w("Project config already loaded");
    return;
  }

  /* Device Config */
  this->config.device.OTAPassword =
      getString("OTAPassword", "12345678").c_str();
  this->config.device.OTAPort = getInt("OTAPort", 3232);

  /* MDNS Config */
  this->config.mdns.hostname = getString("hostname", _mdnsName.c_str()).c_str();
  this->config.mdns.service = getString("service").c_str();

  /* Wifi TX Power Config */
  this->config.txpower.power = getUInt("power", 52);
  /* WiFi Config */
  int networkCount = getInt("networkCount", 0);
  std::string name = "name";
  std::string ssid = "ssid";
  std::string password = "pass";
  std::string channel = "channel";
  std::string power = "power";
  for (int i = 0; i < networkCount; i++) {
    char buffer[2];
    std::string iter_str = Helpers::itoa(i, buffer, 10);

    name.append(iter_str);
    ssid.append(iter_str);
    password.append(iter_str);
    channel.append(iter_str);
    power.append(iter_str);

    const std::string& temp_1 = getString(name.c_str()).c_str();
    const std::string& temp_2 = getString(ssid.c_str()).c_str();
    const std::string& temp_3 = getString(password.c_str()).c_str();
    uint8_t temp_4 = getUInt(channel.c_str());
    uint8_t temp_5 = getUInt(power.c_str());

    //! push_back creates a copy of the object, so we need to use emplace_back
    this->config.networks.emplace_back(
        temp_1, temp_2, temp_3, temp_4, temp_5,
        false);  // false because the networks we store in the config are the
                 // ones we want the esp to connect to, rather than host as AP
  }

  /* AP Config */
  this->config.ap_network.ssid = getString("apSSID").c_str();
  this->config.ap_network.password = getString("apPass").c_str();
  this->config.ap_network.channel = getUInt("apChannel");

  /* Camera Config */
  this->config.camera.vflip = getInt("vflip", 0);
  this->config.camera.href = getInt("href", 0);
  this->config.camera.framesize = getInt("framesize", 4);
  this->config.camera.quality = getInt("quality", 7);
  this->config.camera.brightness = getInt("brightness", 2);

  this->_already_loaded = true;
  this->notify(ObserverEvent::configLoaded);
}

//**********************************************************************************************************************
//*
//!                                                DeviceConfig
//*
//**********************************************************************************************************************
void ProjectConfig::setDeviceConfig(const std::string& OTAPassword,
                                    int OTAPort,
                                    const std::string& binaryName,
                                    bool shouldNotify) {
  log_d("Updating device config");
  this->config.device.OTAPassword.assign(OTAPassword);
  this->config.device.OTAPort = OTAPort;
  // check if binary name is empty
  if (binaryName.empty())
    log_w("Binary name is empty, using previous value");
  else
    this->config.device.binaryName.assign(binaryName);

  if (shouldNotify)
    this->notify(ObserverEvent::deviceConfigUpdated);
}

void ProjectConfig::setMDNSConfig(const std::string& hostname,
                                  const std::string& service,
                                  bool shouldNotify) {
  log_d("Updating MDNS config");
  this->config.mdns.hostname.assign(hostname);
  this->config.mdns.service.assign(service);

  if (shouldNotify)
    this->notify(ObserverEvent::mdnsConfigUpdated);
}

void ProjectConfig::setCameraConfig(uint8_t* vflip,
                                    uint8_t* framesize,
                                    uint8_t* href,
                                    uint8_t* quality,
                                    uint8_t* brightness,
                                    bool shouldNotify) {
  log_d("Updating camera config");
  this->config.camera.vflip = *vflip;
  this->config.camera.href = *href;
  this->config.camera.framesize = *framesize;
  this->config.camera.quality = *quality;
  this->config.camera.brightness = *brightness;

  log_d("Updating Camera config");
  if (shouldNotify)
    this->notify(ObserverEvent::cameraConfigUpdated);
}

void ProjectConfig::setWifiConfig(const std::string& networkName,
                                  const std::string& ssid,
                                  const std::string& password,
                                  uint8_t* channel,
                                  uint8_t* power,
                                  bool adhoc,
                                  bool shouldNotify) {
  // we store the ADHOC flag as false because the networks we store in the
  // config are the ones we want the esp to connect to, rather than host as AP,
  // and here we're just updating them
  size_t size = this->config.networks.size();

  // we're allowing to store up to three additional networks
  if (size == 0) {
    Serial.println("No networks, We're adding a new network");
    this->config.networks.emplace_back(networkName, ssid, password, *channel,
                                       *power, false);
  }

  int networkToUpdate = -1;
  for (int i = 0; i < size; i++) {
    if (this->config.networks[i].name == networkName) {
      // we've found a preexisting network, let's upate it
      networkToUpdate = i;
      break;
    }
  }

  if (networkToUpdate >= 0) {
    this->config.networks[networkToUpdate].name = networkName;
    this->config.networks[networkToUpdate].ssid = ssid;
    this->config.networks[networkToUpdate].password = password;
    this->config.networks[networkToUpdate].channel = *channel;
    this->config.networks[networkToUpdate].power = *power;
    this->config.networks[networkToUpdate].adhoc = false;
  } else if (size < 3) {
    Serial.println("We're adding a new network");
    // we don't have that network yet, we can add it as we still have some space
    // we're using emplace_back as push_back will create a copy of it, we want
    // to avoid that
    this->config.networks.emplace_back(networkName, ssid, password, *channel,
                                       *power, false);
  }

  if (shouldNotify)
    this->notify(ObserverEvent::networksConfigUpdated);
}

void ProjectConfig::deleteWifiConfig(const std::string& networkName,
                                     bool shouldNotify) {
  size_t size = this->config.networks.size();
  if (size == 0) {
    Serial.println("No networks, nothing to delete");
  }

  int networkToDelete = -1;
  for (int i = 0; i < size; i++) {
    if (networkName == this->config.networks[i].name) {
      // we've found a preexisting network, let's upate it
      networkToDelete = i;
      break;  // we can break here as we're not allowing duplicate names
    }
  }

  if (networkToDelete >= 0) {
    this->config.networks.erase(this->config.networks.begin() +
                                networkToDelete);
  }

  if (shouldNotify)
    this->notify(ObserverEvent::networksConfigUpdated);
}

void ProjectConfig::setWiFiTxPower(uint8_t* power, bool shouldNotify) {
  this->config.txpower.power = *power;

  log_d("Updating wifi tx power");
  if (shouldNotify)
    this->notify(ObserverEvent::wifiTxPowerUpdated);
}

void ProjectConfig::setAPWifiConfig(const std::string& ssid,
                                    const std::string& password,
                                    uint8_t* channel,
                                    bool adhoc,
                                    bool shouldNotify) {
  this->config.ap_network.ssid.assign(ssid);
  this->config.ap_network.password.assign(password);
  this->config.ap_network.channel = *channel;
  this->config.ap_network.adhoc = adhoc;

  log_d("Updating access point config");
  if (shouldNotify)
    this->notify(ObserverEvent::networksConfigUpdated);
}

std::string ProjectConfig::DeviceConfig_t::toRepresentation() {
  std::string json = Helpers::format_string(
      "\"device_config\": {\"OTAPassword\": \"%s\", \"OTAPort\": %u}",
      this->OTAPassword.c_str(), this->OTAPort);
  return json;
}

std::string ProjectConfig::MDNSConfig_t::toRepresentation() {
  std::string json = Helpers::format_string(
      "\"mdns_config\": {\"hostname\": \"%s\", \"service\": \"%s\"}",
      this->hostname.c_str(), this->service.c_str());
  return json;
}

std::string ProjectConfig::CameraConfig_t::toRepresentation() {
  std::string json = Helpers::format_string(
      "\"camera_config\": {\"vflip\": %d,\"framesize\": %d,\"href\": "
      "%d,\"quality\": %d,\"brightness\": %d}",
      this->vflip, this->framesize, this->href, this->quality,
      this->brightness);
  return json;
}

std::string ProjectConfig::WiFiConfig_t::toRepresentation() {
  std::string json = Helpers::format_string(
      "{\"name\": \"%s\", \"ssid\": \"%s\", \"password\": \"%s\", "
      "\"channel\": "
      "%u, \"power\": %u,\"adhoc\": %s}",
      this->name.c_str(), this->ssid.c_str(), this->password.c_str(),
      this->channel, this->power, this->adhoc ? "true" : "false");
  return json;
}

std::string ProjectConfig::AP_WiFiConfig_t::toRepresentation() {
  std::string json = Helpers::format_string(
      "\"ap_wifi_config\": {\"ssid\": \"%s\", \"password\": \"%s\", "
      "\"channel\": %u, \"adhoc\": %s}",
      this->ssid.c_str(), this->password.c_str(), this->channel,
      this->adhoc ? "true" : "false");
  return json;
}

std::string ProjectConfig::WiFiTxPower_t::toRepresentation() {
  std::string json =
      Helpers::format_string("\"wifi_tx_power\": {\"power\": %u}", this->power);
  return json;
}

//**********************************************************************************************************************
//*
//!                                                Get Methods
//*
//**********************************************************************************************************************

ProjectConfig::DeviceConfig_t* ProjectConfig::getDeviceConfig() {
  return &this->config.device;
}
ProjectConfig::CameraConfig_t* ProjectConfig::getCameraConfig() {
  return &this->config.camera;
}
std::vector<ProjectConfig::WiFiConfig_t>* ProjectConfig::getWifiConfigs() {
  return &this->config.networks;
}
ProjectConfig::AP_WiFiConfig_t* ProjectConfig::getAPWifiConfig() {
  return &this->config.ap_network;
}
ProjectConfig::MDNSConfig_t* ProjectConfig::getMDNSConfig() {
  return &this->config.mdns;
}
ProjectConfig::WiFiTxPower_t* ProjectConfig::getWiFiTxPowerConfig() {
  return &this->config.txpower;
}
