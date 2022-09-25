#ifndef STATEMANAGER_HPP
#define STATEMANAGER_HPP
#include <Arduino.h>

/*
 * StateManager
 * All Project States are managed here
 */
struct DeviceStates
{
    enum State_e
    {
        Starting,
        Started,
        Stopping,
        Stopped,
        Error
    };

    enum LEDStates_e
    {
        _LEDOff,
        _LEDOn,
        _LEDBlink,
        _SerialManager_Start,
        _SerialManager_Stop,
        _SerialManager_Error,
        _WiFiState_None,
        _WiFiState_Connecting,
        _WiFiState_Connected,
        _WiFiState_Disconnected,
        _WiFiState_Disconnecting,
        _WiFiState_ADHOC,
        _WiFiState_Error,
        _WebServerState_None,
        _WebServerState_Starting,
        _WebServerState_Started,
        _WebServerState_Stopping,
        _WebServerState_Stopped,
        _WebServerState_Error,
        _MDNSState_None,
        _MDNSState_Starting,
        _MDNSState_Started,
        _MDNSState_Stopping,
        _MDNSState_Stopped,
        _MDNSState_Error,
        _Camera_Success,
        _Camera_Connected,
        _Camera_Disconnected,
        _Camera_Error,
        _Stream_OFF,
        _Stream_ON,
        _Stream_Error,
    };

    enum WiFiState_e
    {
        WiFiState_None,
        WiFiState_Connecting,
        WiFiState_Connected,
        WiFiState_Disconnected,
        WiFiState_Disconnecting,
        WiFiState_ADHOC,
        WiFiState_Error
    };

    enum WebServerState_e
    {
        WebServerState_None,
        WebServerState_Starting,
        WebServerState_Started,
        WebServerState_Stopping,
        WebServerState_Stopped,
        WebServerState_Error
    };

    enum MDNSState_e
    {
        MDNSState_None,
        MDNSState_Starting,
        MDNSState_Started,
        MDNSState_Stopping,
        MDNSState_Stopped,
        MDNSState_Error
    };

    enum CameraState_e
    {
        Camera_Success,
        Camera_Connected,
        Camera_Disconnected,
        Camera_Error
    };

    enum StreamState_e
    {
        Stream_OFF,
        Stream_ON,
        Stream_Error
    };
};

/*
 * EventManager
 * All Project Events are managed here
 */
template <class T>
class StateManager
{
public:
    StateManager() {}

    virtual ~StateManager() {}

    void setState(T state)
    {
        _current_state = state;
    }

    /*
     * Get States
     * Returns the current state of the device
     */
    T getCurrentState()
    {
        return _current_state;
    }

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

extern StateManager<State_e> stateManager;
extern StateManager<WiFiState_e> wifiStateManager;
extern StateManager<WebServerState_e> webServerStateManager;
extern StateManager<MDNSState_e> mdnsStateManager;
extern StateManager<CameraState_e> cameraStateManager;
extern StateManager<LEDStates_e> ledStateManager;
extern StateManager<StreamState_e> streamStateManager;

#endif // STATEMANAGER_HPP