#ifndef IMPROV_HANDLER_HPP
#define IMPROV_HANDLER_HPP
#include "data/config/project_config.hpp"
#include "improv.h"

class ImprovHandler {
  ProjectConfig& projectConfig;
  uint8_t _buffer[16];
  uint8_t _position;

  void onErrorCallback(improv::Error err);
  bool onCommandCallback(improv::ImprovCommand cmd);
  void getNetworks();
  void send_response(std::vector<uint8_t>& response);
  void set_state(improv::State state);
  void set_error(improv::Error error);
  std::vector<std::string> getLocalUrl();
  bool connectToNetwork(const std::string ssid, const std::string password);

 public:
  ImprovHandler(ProjectConfig& projectConfig);
  ~ImprovHandler();

  void loop();
};

#endif  // IMPROV_HANDLER_HPP
