#include "StateManager.h"

namespace OpenIris{
    void StateManager::setState(State state){
        current_state=state;
    }

    State StateManager::getCurrentState(){
        return current_state;
    }
}