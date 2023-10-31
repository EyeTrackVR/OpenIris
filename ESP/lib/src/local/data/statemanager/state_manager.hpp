#ifndef LOCAL_STATEMANAGER_HPP
#define LOCAL_STATEMANAGER_HPP
#include <data/statemanager/state_manager.hpp>

/*
 * StateManager
 * All Project States are managed here
 */
struct DeviceStates {
  enum LEDStates_e {
    LedStateNone,
    WebServerState_Error,
    WiFiState_Error,
    MDNSState_Error,
    Camera_Error,
    WiFiState_Connecting,
    WiFiState_Connected
  };

  enum ConfigState_e {
    cameraConfigLoad,
    cameraConfigSave,
    cameraConfigReset,
    cameraConfigUpdate,
    cameraConfigError,
  };

  enum CameraState_e {
    Camera_Disconnected,
    Camera_Success,
    Camera_Connected,
    _Camera_Error
  };

  enum StreamState_e { Stream_OFF, Stream_ON, Stream_Error };
};

typedef DeviceStates::CameraState_e CameraState_e;
typedef DeviceStates::LEDStates_e LEDStates_e;
typedef DeviceStates::StreamState_e StreamState_e;
typedef DeviceStates::ConfigState_e ConfigState_e;

extern StateManager<CameraState_e> cameraStateManager;
extern StateManager<LEDStates_e> ledStateManager;
extern StateManager<StreamState_e> streamStateManager;
extern StateManager<ConfigState_e> configStateManager;

#endif  // STATEMANAGER_HPP
