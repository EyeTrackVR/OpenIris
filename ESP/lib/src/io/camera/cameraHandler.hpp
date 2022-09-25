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
	int setCameraResolution(framesize_t frameSize);
	int setVFlip(int direction);
	int setHFlip(int direction);
	int setVieWindow(int offsetX, int offsetY, int outputX, int outputY);
	void update(ObserverEvent::Event event);
	void resetCamera(bool type = 0);

private:
	void loadConfigData();
	bool setupCamera();
	void setupCameraPinout();
	void setupBasicResolution();
	void setupCameraSensor();
};
