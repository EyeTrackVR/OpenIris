#include "cameraHandler.h"

namespace CameraHandler{
    sensor_t* camera_sensor = NULL;
    
    int setupCamera(){
        Serial.print("Setting up camera \r\n");

        camera_config_t config;
        config.ledc_channel = LEDC_CHANNEL_0;
        config.ledc_timer = LEDC_TIMER_0;
        config.pin_d0 = Y2_GPIO_NUM;
        config.pin_d1 = Y3_GPIO_NUM;
        config.pin_d2 = Y4_GPIO_NUM;
        config.pin_d3 = Y5_GPIO_NUM;
        config.pin_d4 = Y6_GPIO_NUM;
        config.pin_d5 = Y7_GPIO_NUM;
        config.pin_d6 = Y8_GPIO_NUM;
        config.pin_d7 = Y9_GPIO_NUM;
        config.pin_xclk = XCLK_GPIO_NUM;
        config.pin_pclk = PCLK_GPIO_NUM;
        config.pin_vsync = VSYNC_GPIO_NUM;
        config.pin_href = HREF_GPIO_NUM;
        config.pin_sscb_sda = SIOD_GPIO_NUM;
        config.pin_sscb_scl = SIOC_GPIO_NUM;
        config.pin_pwdn = PWDN_GPIO_NUM;
        config.pin_reset = RESET_GPIO_NUM;
        config.xclk_freq_hz = 20000000;
        config.pixel_format = PIXFORMAT_JPEG;

        if (psramFound()){
            Serial.println("Found psram, setting the UXGA image quality");
            config.frame_size = FRAMESIZE_240X240;
            config.jpeg_quality = 10;
            config.fb_count = 2;
        }else{
            Serial.println("Did not find psram, setting svga quality");
            config.frame_size = FRAMESIZE_SVGA;
            config.jpeg_quality = 12;
            config.fb_count = 1;
        }

        esp_err_t  err = esp_camera_init(&config);

        camera_sensor = esp_camera_sensor_get();

        if (err != ESP_OK){
            Serial.printf("Camera initialization failed with error: 0x%x \r\n", err);
            return -1;
        }else{
            Serial.println("Sucessfully initialized the camera!");
            return 0;
        }
    }

    int setCameraResolution(framesize_t framesize){
        if(camera_sensor->pixformat == PIXFORMAT_JPEG){
            try{
            return camera_sensor->set_framesize(camera_sensor, framesize);
            }catch(...){
                // they sent us a malformed or unsupported framesize - rather than crash - tell them about it
                return -1;
            }
        }
        return -1;
    }

    int setVFlip(int direction){
        return camera_sensor->set_vflip(camera_sensor, direction);
    }

    int setHFlip(int direction){
        return camera_sensor->set_hmirror(camera_sensor, direction);
    }
}