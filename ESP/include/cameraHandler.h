#pragma once
#include "pinout.h"
#include "esp_camera.h"
#include <Arduino.h>

namespace OpenIris
{
    class CameraHandler
    {
    private:
        sensor_t *camera_sensor;
        camera_config_t config;

    public:
        int setupCamera();
        int setCameraResolution(framesize_t frameSize);
        int setVFlip(int direction);
        int setHFlip(int direction);
        int setVieWindow(int offsetX, int offsetY, int outputX, int outputY);
    };
}