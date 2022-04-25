#include "cameraHandler.h"

int OpenIris::CameraHandler::setupCamera()
{
  Serial.print("Setting up camera \r\n");

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

  if (psramFound())
  {
    Serial.println("Found psram, setting the 176x144 image quality");
    config.frame_size = FRAMESIZE_QCIF;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  }
  else
  {
    Serial.println("Did not find psram, setting svga quality");
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

  esp_err_t err = esp_camera_init(&config);

  camera_sensor = esp_camera_sensor_get();
  camera_sensor->set_special_effect(camera_sensor, 2);

  if (err != ESP_OK)
  {
    Serial.printf("Camera initialization failed with error: 0x%x \r\n", err);
    // TODO add led blinking here
    return -1;
  }
  else
  {
    Serial.println("Sucessfully initialized the camera!");
    // TODO add led blinking here
    return 0;
  }
}

int OpenIris::CameraHandler::setCameraResolution(framesize_t frameSize)
{
  if (camera_sensor->pixformat == PIXFORMAT_JPEG)
  {
    try
    {
      return camera_sensor->set_framesize(camera_sensor, frameSize);
    }
    catch (...)
    {
      // they sent us a malformed or unsupported frameSize - rather than crash - tell them about it
      return -1;
    }
  }
  return -1;
}

int OpenIris::CameraHandler::setVFlip(int direction)
{
  return camera_sensor->set_vflip(camera_sensor, direction);
}

int OpenIris::CameraHandler::setHFlip(int direction)
{
  return camera_sensor->set_hmirror(camera_sensor, direction);
}

int OpenIris::CameraHandler::setVieWindow(int offsetX, int offsetY, int outputX, int outputY)
{
  // we're only providing these parameters as these are the only ones that actually affect the ov2640
  // TODO: if we're going to support different sensors - this will have to be moved to per-sensor implementation 'cause
  // TODO: manufacturers handle it differently each time

  // we're doubling the totalX and totalY parameters to make it easier for end user to adjust the eye ROI
  // it also seems to produce a cleaner image
  return camera_sensor->set_res_raw(camera_sensor, 0, 0, 0, 0, offsetX, offsetY, outputX * 2, outputY * 2, outputX, outputY, true, true);
}