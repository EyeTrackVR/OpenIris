#ifndef LOCAL_STATEMANAGER_HPP
#define LOCAL_STATEMANAGER_HPP
#include <data/statemanager/state_manager.hpp>

/*
 * StateManager
 * All Project States are managed here
 */
struct DeviceStates {
  enum LEDStates_e {
    _LedStateNone,
   /*  _Improv_Start,
    _Improv_Stop,
    _Improv_Processing,
    _Improv_Error, */
    _WebServerState_Error,
    _WiFiState_Error,
    _MDNSState_Error,
    _Camera_Error,
    _WiFiState_Connecting,
    _WiFiState_Connected
  };

  enum ConfigState_e {
    configLoaded,
    deviceConfigUpdated,
    mdnsConfigUpdated,
    networksConfigUpdated,
    apConfigUpdated,
    wifiTxPowerUpdated,
    cameraConfigUpdated
  };

  enum CameraState_e {
    Camera_Disconnected,
    Camera_Success,
    Camera_Connected,
    Camera_Error
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
