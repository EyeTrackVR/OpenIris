#include "improvHandler.hpp"
#include "data/StateManager/StateManager.hpp"
#include "data/utilities/network_utilities.hpp"

ImprovHandler::ImprovHandler(ProjectConfig& projectConfig)
    : projectConfig(projectConfig), _buffer{0}, _position(0) {}

ImprovHandler::~ImprovHandler() {}

void ImprovHandler::onErrorCallback(improv::Error err) {
  ledStateManager.setState(LEDStates_e::_Improv_Error);
}

bool ImprovHandler::onCommandCallback(improv::ImprovCommand cmd) {
  switch (cmd.command) {
    case improv::Command::GET_CURRENT_STATE: {
      auto wifiConfigs = projectConfig.getWifiConfigs();
      if (wifiStateManager.getCurrentState() ==
          WiFiState_e::WiFiState_Connected) {
        this->set_state(improv::State::STATE_PROVISIONED);
        std::vector<uint8_t> data = improv::build_rpc_response(
            improv::GET_CURRENT_STATE, this->getLocalUrl(), false);
        this->send_response(data);
        break;
      }
      this->set_state(improv::State::STATE_AUTHORIZED);
      break;
    }

    case improv::Command::WIFI_SETTINGS: {
      ledStateManager.setState(LEDStates_e::_Improv_Start);

      if (cmd.ssid.empty() || cmd.ssid.size() == 0) {
        this->set_error(improv::Error::ERROR_INVALID_RPC);
        break;
      }

      ledStateManager.setState(LEDStates_e::_Improv_Processing);
      this->set_state(improv::STATE_PROVISIONING);
      Network_Utilities::my_delay(0.5);
      if (this->connectToNetwork(cmd.ssid, cmd.password)) {
        this->set_state(improv::State::STATE_PROVISIONED);
        std::vector<uint8_t> data = improv::build_rpc_response(
            improv::GET_CURRENT_STATE, this->getLocalUrl(), false);
        this->send_response(data);

        ledStateManager.setState(LEDStates_e::_Improv_Stop);
      } else {
        this->set_state(improv::STATE_STOPPED);
        this->set_error(improv::Error::ERROR_UNABLE_TO_CONNECT);
      }
      break;
    }

    case improv::Command::GET_DEVICE_INFO: {
      std::vector<std::string> infos = {"OpenIris", VERSION, CAMERA_MODULE_NAME,
                                        "EyeTrackVR"};
      std::vector<uint8_t> data =
          improv::build_rpc_response(improv::GET_DEVICE_INFO, infos, false);
      this->send_response(data);
      break;
    }

    case improv::Command::GET_WIFI_NETWORKS: {
      this->getNetworks();
      break;
    }

    default: {
      this->set_error(improv::ERROR_UNKNOWN_RPC);
      return false;
    }
  }

  return true;
}

void ImprovHandler::getNetworks() {
  Network_Utilities::setupWifiScan();
  int networkNum = WiFi.scanNetworks(false, true);
  for (int id = 0; id < networkNum; ++id) {
    std::vector<uint8_t> data = improv::build_rpc_response(
        improv::GET_WIFI_NETWORKS,
        {WiFi.SSID(id), String(WiFi.RSSI(id)),
         (WiFi.encryptionType(id) == WIFI_AUTH_OPEN ? "NO" : "YES")},
        false);
    this->send_response(data);
    delay(1);
  }
  // final response
  std::vector<uint8_t> data = improv::build_rpc_response(
      improv::GET_WIFI_NETWORKS, std::vector<std::string>{}, false);
  this->send_response(data);
}

void ImprovHandler::set_state(improv::State state) {
  std::vector<uint8_t> data = {'I', 'M', 'P', 'R', 'O', 'V'};
  data.resize(11);
  data[6] = improv::IMPROV_SERIAL_VERSION;
  data[7] = improv::TYPE_CURRENT_STATE;
  data[8] = 1;
  data[9] = state;

  uint8_t checksum = 0x00;
  for (uint8_t d : data)
    checksum += d;
  data[10] = checksum;

  Serial.write(data.data(), data.size());
}

void ImprovHandler::send_response(std::vector<uint8_t>& response) {
  std::vector<uint8_t> data = {'I', 'M', 'P', 'R', 'O', 'V'};
  data.resize(9);
  data[6] = improv::IMPROV_SERIAL_VERSION;
  data[7] = improv::TYPE_RPC_RESPONSE;
  data[8] = response.size();
  data.insert(data.end(), response.begin(), response.end());

  uint8_t checksum = 0x00;
  for (uint8_t d : data)
    checksum += d;
  data.push_back(checksum);

  Serial.write(data.data(), data.size());
}

void ImprovHandler::set_error(improv::Error error) {
  std::vector<uint8_t> data = {'I', 'M', 'P', 'R', 'O', 'V'};
  data.resize(11);
  data[6] = improv::IMPROV_SERIAL_VERSION;
  data[7] = improv::TYPE_ERROR_STATE;
  data[8] = 1;
  data[9] = error;

  uint8_t checksum = 0x00;
  for (uint8_t d : data)
    checksum += d;
  data[10] = checksum;

  Serial.write(data.data(), data.size());
}

std::vector<std::string> ImprovHandler::getLocalUrl() {
  //* Construct URL
  std::string root_url("http://" +
                       std::string(WiFi.localIP().toString().c_str()));
  std::vector<std::string> url = {root_url};
  return url;
}

bool ImprovHandler::connectToNetwork(const std::string ssid,
                                     const std::string password) {
  //* Save the config to flash
  projectConfig.setWifiConfig(ssid, ssid, password, 10, 52, false, true);
  return (wifiStateManager.getCurrentState() ==
          WiFiState_e::WiFiState_Connected);
}

void ImprovHandler::loop() {
  if (Serial.available() > 0) {
    uint8_t b = Serial.read();

    if (improv::parse_improv_serial_byte(
            _position, b, _buffer,
            [&](improv::ImprovCommand cmd) -> bool {
              return this->onCommandCallback(cmd);
            },
            [&](improv::Error error) { this->onErrorCallback(error); })) {
      _buffer[_position++] = b;
    } else {
      _position = 0;
    }
  }
}
