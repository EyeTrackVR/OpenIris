#ifndef MDNSHANDLER_HPP
#define MDNSHANDLER_HPP
#include <ESPmDNS.h>
#include "data/StateManager/StateManager.hpp"
#include "data/config/project_config.hpp"
#include "data/utilities/Observer.hpp"
#include "data/utilities/helpers.hpp"

class MDNSHandler : public IObserver<ConfigState_e> {
 private:
  ProjectConfig* configManager;

 public:
  MDNSHandler(ProjectConfig* configManager);
  bool startMDNS();
  void update(ConfigState_e event) override;
  std::string getName() override;
};

#endif  // MDNSHANDLER_HPP
