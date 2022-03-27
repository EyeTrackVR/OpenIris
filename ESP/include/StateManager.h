#pragma once

namespace OpenIris{
    enum State {
            Starting = 1,
            ConnectingToWifi = 2,
            ConnectingToWifiError = 3,
            ConnectingToWifiSuccess = 4,
            ServerError = 5,
            CameraError = 6,
            MDNSSuccess = 7,
            MDNSError = 8,
    };

    class StateManager{
        public:
            StateManager() : current_state(Starting) {}
            void setState(State state);
            State getCurrentState();
        private:
            State current_state;
    };

}