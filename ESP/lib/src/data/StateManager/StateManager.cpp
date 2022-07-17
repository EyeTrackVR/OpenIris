#include "StateManager.hpp"

template <class T>
StateManager<T>::StateManager() {}

template <class T>
StateManager<T>::~StateManager() {}

template <class T>
void StateManager<T>::setState(T state)
{
    _current_state = state;
}

/*
 * Get States
 * Returns the current state of the device
 */
template <class T>
T StateManager<T>::getCurrentState()
{
    return _current_state;
}