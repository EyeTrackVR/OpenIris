#ifndef MDNSHANDLER_HPP
#define MDNSHANDLER_HPP
#include <ESPmDNS.h>
#include "data/StateManager/StateManager.hpp"
#include "data/utilities/Observer.hpp"
#include "data/utilities/helpers.hpp"
#include "data/config/project_config.hpp"

class MDNSHandler : public IObserver
{
private:
	StateManager<MDNSState_e> *stateManager;
	ProjectConfig *configManager;

public:
	MDNSHandler(StateManager<MDNSState_e> *stateManager,
				ProjectConfig *configManager);
	bool startMDNS();
	void update(ObserverEvent::Event event) override;
};

#endif // MDNSHANDLER_HPP