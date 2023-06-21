#include "tasks.hpp"

void OpenIrisTasks::ScheduleRestart(int milliseconds) {
    yield();
    int initialTime = millis();
    while (millis() - initialTime <= milliseconds) {
        continue;
    }
    yield();
    ESP.restart();
}
