#ifndef STATEMANAGER_HPP
#define STATEMANAGER_HPP
#include <Arduino.h>

/*
 * StateManager
 * All Project States are managed here
 */
struct DeviceStates {
  enum LEDStates_e {
    _LedStateNone,
    _Improv_Start,
    _Improv_Stop,
    _Improv_Processing,
    _Improv_Error,
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

  enum WiFiState_e {
    WiFiState_None,
    WiFiState_Idle,
    WiFiState_Disconnected,
    WiFiState_Connecting,
    WiFiState_Connected,
    WiFiState_ADHOC,
    WiFiState_Error
  };

  enum WebServerState_e {
    WebServerState_Stopped,
    WebServerState_Starting,
    WebServerState_Started,
    WebServerState_Stopping,
    WebServerState_Error
  };

  enum MDNSState_e {
    MDNSState_Stopped,
    MDNSState_Starting,
    MDNSState_Started,
    MDNSState_Stopping,
    MDNSState_Error,
    MDNSState_QueryStarted,
    MDNSState_QueryComplete
  };

  enum CameraState_e {
    Camera_Disconnected,
    Camera_Success,
    Camera_Connected,
    Camera_Error
  };

  enum StreamState_e { Stream_OFF, Stream_ON, Stream_Error };
  enum State_e { Starting, Started, Stopping, Stopped, Error };
};

/*
 * EventManager
 * All Project Events are managed here
 */
template <class T>
class StateManager {
 public:
  StateManager() { this->_current_state = static_cast<T>(0); }

  virtual ~StateManager() {}

  /*
   * @brief Sets the  state of the stateManager
   * @param T state - the state to be set
   */
  void setState(T state) { _current_state = state; }

  /*
   * @brief Returns the current state of the stateManager
   */
  T getCurrentState() { return _current_state; }

 private:
  T _current_state;
};

typedef DeviceStates::State_e State_e;
typedef DeviceStates::WiFiState_e WiFiState_e;
typedef DeviceStates::WebServerState_e WebServerState_e;
typedef DeviceStates::MDNSState_e MDNSState_e;
typedef DeviceStates::CameraState_e CameraState_e;
typedef DeviceStates::LEDStates_e LEDStates_e;
typedef DeviceStates::StreamState_e StreamState_e;
typedef DeviceStates::ConfigState_e ConfigState_e;

extern StateManager<State_e> stateManager;
extern StateManager<WiFiState_e> wifiStateManager;
extern StateManager<WebServerState_e> webServerStateManager;
extern StateManager<MDNSState_e> mdnsStateManager;
extern StateManager<CameraState_e> cameraStateManager;
extern StateManager<LEDStates_e> ledStateManager;
extern StateManager<StreamState_e> streamStateManager;
extern StateManager<ConfigState_e> configStateManager;

#endif  // STATEMANAGER_HPP
