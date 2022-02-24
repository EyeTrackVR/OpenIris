#include "camera_pins.h"
#include "esp_camera.h"
#include <Arduino.h>


void setupCamera(){
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

    if (err != ESP_OK){
        Serial.printf("Camera initialization failed with error: 0x%x \r\n", err);
        return;
    }else{
        Serial.println("Sucessfully initialized the camera!");
    }
}