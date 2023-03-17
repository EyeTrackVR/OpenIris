#ifndef IMPROV_HANDLER_HPP
#define IMPROV_HANDLER_HPP
#include "data/StateManager/StateManager.hpp"
#include "data/config/project_config.hpp"
#include "data/utilities/Observer.hpp"
#include "data/utilities/network_utilities.hpp"
#include "improv.h"

class ImprovHandler {
  ProjectConfig* projectConfig;
  uint8_t _buffer[15];  // TODO: is 15 enough?
  uint8_t _position;

  void onErrorCallback(improv::Error err);
  bool onCommandCallback(improv::ImprovCommand cmd);
  void getNetworks();
  void send_response(std::vector<uint8_t>& response);
  void set_state(improv::State state);
  void set_error(improv::Error error);

 public:
  ImprovHandler(ProjectConfig* projectConfig);
  ~ImprovHandler();

  void loop();
};

#endif  // IMPROV_HANDLER_HPP
