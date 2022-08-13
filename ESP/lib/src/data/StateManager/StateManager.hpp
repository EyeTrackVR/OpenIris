#ifndef STATEMANAGER_HPP
#define STATEMANAGER_HPP
#include <Arduino.h>

/*
 * StateManager
 * All Project States are managed here
 */
class ProgramStates
{
public:
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

        enum ButtonState_e
        {
            Buttons_OFF,
            Buttons_ON,
            Buttons_PLUS,
            Buttons_MINUS,
            Buttons_Error
        };

        enum StreamState_e
        {
            Stream_OFF,
            Stream_ON,
            Stream_Error
        };
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

typedef ProgramStates::DeviceStates::State_e State_e;
typedef ProgramStates::DeviceStates::WiFiState_e WiFiState_e;
typedef ProgramStates::DeviceStates::WebServerState_e WebServerState_e;
typedef ProgramStates::DeviceStates::MDNSState_e MDNSState_e;
typedef ProgramStates::DeviceStates::CameraState_e CameraState_e;
typedef ProgramStates::DeviceStates::ButtonState_e ButtonState_e;
typedef ProgramStates::DeviceStates::StreamState_e StreamState_e;

extern StateManager<State_e> stateManager;
extern StateManager<WiFiState_e> wifiStateManager;
extern StateManager<WebServerState_e> webServerStateManager;
extern StateManager<MDNSState_e> mdnsStateManager;
extern StateManager<CameraState_e> cameraStateManager;
extern StateManager<ButtonState_e> buttonStateManager;
extern StateManager<StreamState_e> streamStateManager;


#endif // STATEMANAGER_HPP