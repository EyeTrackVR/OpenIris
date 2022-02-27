#include "pinout.h"
#include "esp_camera.h"
#include <Arduino.h>


namespace CameraHandler{

    extern sensor_t* camera_sensor;

    int setupCamera();
    int setCameraResolution(framesize_t framesize);
    int setVFlip(int direction);
    int setHFlip(int direction);
}