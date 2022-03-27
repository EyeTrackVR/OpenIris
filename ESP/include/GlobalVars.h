#pragma once
#ifndef GLOBALVARS_H
#define GLOBALVARS_H

#include "StateManager.h"
#include "LEDManager.h"
#include "cameraHandler.h"
extern char* MDSNTrackerName;
extern int STREAM_SERVER_PORT;
extern int CONTROL_SERVER_PORT;

extern OpenIris::LEDManager ledManager;
extern OpenIris::StateManager stateManager;
extern OpenIris::CameraHandler cameraHandler;

#endif