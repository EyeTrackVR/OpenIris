#include "StateManager.hpp"

StateManager<ProgramStates::DeviceStates::State_e> stateManager;
StateManager<ProgramStates::DeviceStates::WiFiState_e> wifiStateManager;
StateManager<ProgramStates::DeviceStates::WebServerState_e> webServerStateManager;
StateManager<ProgramStates::DeviceStates::MDNSState_e> mdnsStateManager;
StateManager<ProgramStates::DeviceStates::CameraState_e> cameraStateManager;
StateManager<ProgramStates::DeviceStates::ButtonState_e> buttonStateManager;
StateManager<ProgramStates::DeviceStates::StreamState_e> streamStateManager;