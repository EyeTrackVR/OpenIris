#include "tasks.hpp"

void OpenIrisTasks::ScheduleRestart(int milliseconds) {
    int initialTime = millis(); 
    while (millis() - initialTime <= milliseconds) {
        continue;
    }
    ESP.restart();
}