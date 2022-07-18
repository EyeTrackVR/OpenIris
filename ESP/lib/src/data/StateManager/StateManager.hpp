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


#endif // STATEMANAGER_HPP