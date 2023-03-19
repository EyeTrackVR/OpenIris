#pragma once
#include <Arduino.h>
#include <esp_camera.h>
#include "data/utilities/Observer.hpp"
#include "data/utilities/network_utilities.hpp"
#include "data/config/project_config.hpp"
#include "data/StateManager/StateManager.hpp"

class CameraHandler : public IObserver
{
private:
	sensor_t *camera_sensor;
	camera_config_t config;
	ProjectConfig *configManager;
	StateManager<LEDStates_e> *stateManager;

public:
	CameraHandler(ProjectConfig *configManager, StateManager<LEDStates_e> *stateManager);
	void update(ObserverEvent::Event event) override;
	void resetCamera(bool type = false);

private:
	bool setupCamera();
	void setupCameraPinout();
	void setupResolutionConfiguration();
	void setupCameraSensor();
};
