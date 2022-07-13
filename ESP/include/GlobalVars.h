#pragma once
#ifndef GLOBALVARS_H
#define GLOBALVARS_H

#include "StateManager.h"
#include "LEDManager.h"
#include "cameraHandler.h"
extern char *MDSNTrackerName;
extern int STREAM_SERVER_PORT;
extern int CONTROL_SERVER_PORT;

extern LEDManager ledManager;
extern StateManager stateManager;
extern CameraHandler cameraHandler;

#endif